#include "common/common_pch.h"

#include "common/log/log.h"
#include "common/log/i_log_consumer.h"
#include "common/log/log_consumer_console.h"
#include "common/log/log_consumer_output_debug_string.h"
#include "common/log/log_consumer_write_to_file.h"
#include "common/worker/worker_task.h"
#include "common/util/command_line.h"
#include "common/util/utf8.h"
#include "common/macro.h"

static std::atomic<Log*> s_singleton = nullptr;

class LogImplementation
{
public:
	LogImplementation(const std::vector< std::shared_ptr< ILogConsumer > >& in_log_consumers);
	~LogImplementation();
	void DrainLogMessages();
	const bool AcceptsTopic(const LogTopic in_topic);
	void AddMessage(const LogTopic in_topic, const std::string& in_message);

private:
	void DoWork();

private:
	std::shared_ptr<WorkerTask> _worker_thread;

	std::list< std::pair< LogTopic, std::string > > _list_messages;
	std::mutex _list_messages_mutex;

	//ownership
	std::vector< std::shared_ptr< ILogConsumer > > _log_consumers;

	//cache the consumers for each topic so we don't have to keep on filtering
	std::vector< ILogConsumer*> _topic_log_consumers[(unsigned int)LogTopic::Count];
};

class CheckAllLogMessagesConsumed
{
public:
	~CheckAllLogMessagesConsumed()
	{
		//so, this raises an issue. if we have logged messages before the log is constructed to consume all messages
		// as the application shuts down before messages can ever be consumed, have we done our job, or does log scope need to be eariler, like static startup?
		// or should we never log messages in static startup? at least before log scope...?
		std::lock_guard lock(_list_messages_mutex);
		if (0 != _list_messages.size())
		{
			Log::AddConsole("Warning: dtor of log with %d log messages not consumed", (int)_list_messages.size());
		}
		assert(0 == _list_messages.size());
	}
	void AddMessage(const LogTopic in_topic, const std::string& in_message)
	{
		std::lock_guard lock(_list_messages_mutex);
		_list_messages.push_back(std::pair< LogTopic, std::string >(in_topic, in_message));
	}
	void ConsumeAllMessages(std::list< std::pair< LogTopic, std::string > >& in_list_messages)
	{
		std::lock_guard lock(_list_messages_mutex);
		for (const auto& iter: _list_messages)
		{
			in_list_messages.push_back(iter);
		}
		_list_messages.clear();
	}
	//the unit test may cause logs to emit, but never care about consuming, and no appropriate central place to consume logs for unit test? (todo)
	void RunningUintTest()
	{
		std::lock_guard lock(_list_messages_mutex);
		_list_messages.clear();
	}
public:
	std::list< std::pair< LogTopic, std::string > > _list_messages;
	std::mutex _list_messages_mutex;
};
static CheckAllLogMessagesConsumed s_CheckAllLogMessagesConsumed;

LogImplementation::LogImplementation(const std::vector< std::shared_ptr< ILogConsumer > >& in_log_consumers)
	: _log_consumers(in_log_consumers)
{
	//DSC_ASSERT(nullptr == s_singleton);
	//s_singleton = this;
	_worker_thread = WorkerTask::Factory([=](){
		DoWork();
	});
	//UpdateTopicLogConsumers();
	for (int index = 0; index < (int)LogTopic::Count; ++index)
	{
		for (auto iter : in_log_consumers)
		{
			if (true == iter->AcceptsTopic((LogTopic)index))
			{
				_topic_log_consumers[index].push_back(iter.get());
			}
		}
	}
}

LogImplementation::~LogImplementation()
{
	_worker_thread = nullptr;
}

void LogImplementation::DrainLogMessages()
{
	std::lock_guard< std::mutex > lock(_list_messages_mutex);
	s_CheckAllLogMessagesConsumed.ConsumeAllMessages(_list_messages);
	//normally null check things, ie. m_workerThread
	// in this case, that would allow worker thread to not be created, and silently do a fake consume of log messages
	_worker_thread->SignalWorkToDo();
}

const bool LogImplementation::AcceptsTopic(const LogTopic in_topic)
{
	return (0 != _topic_log_consumers[(unsigned int)in_topic].size());
}

void LogImplementation::AddMessage(const LogTopic in_topic, const std::string& in_message)
{
	{
		std::lock_guard< std::mutex > lock(_list_messages_mutex);
		_list_messages.push_back(std::pair< LogTopic, std::string >(in_topic, in_message));
	}

	if (nullptr != _worker_thread)
	{
		_worker_thread->SignalWorkToDo();
	}
	return;
}

void LogImplementation::DoWork()
{
	while (true)
	{
		std::pair< LogTopic, std::string > messagePair;
		{
			std::lock_guard< std::mutex > lock(_list_messages_mutex);
			if (true == _list_messages.empty())
			{
			return;
			}
			messagePair = _list_messages.front();
			_list_messages.pop_front();
		}

		for (auto pIter : _topic_log_consumers[(unsigned int)messagePair.first])
		{
			pIter->AddMessage(messagePair.first, messagePair.second);
		}
	}

	return;
}

static const std::string FormatString(const char* const pFormat, va_list vaArgs)
{
	// reliably acquire the size
	// from a copy of the variable argument array
	// and a functionally reliable call to mock the formatting
	va_list vaArgsCopy;
	va_copy(vaArgsCopy, vaArgs);
	const int iLen = std::vsnprintf(NULL, 0, pFormat, vaArgsCopy);
	va_end(vaArgsCopy);

	// return a formatted string without risking memory mismanagement
	// and without assuming any compiler or platform specific behavior
	std::vector<char> zc(iLen + 1);
	std::vsnprintf(zc.data(), zc.size(), pFormat, vaArgs);

	std::string message(zc.data(), iLen); 

	return message;
}

void Log::AddConsole(const char* const in_format, ... )
{
	va_list va_args;
	va_start(va_args, in_format);
	std::string message = FormatString(in_format, va_args);
	va_end(va_args);

	message += "\n";
	OutputDebugStringW(Utf8::Utf8ToUtf16(message).c_str());
	//OutputDebugStringW(L"\n");
}

std::shared_ptr< Log > Log::FactoryCommandLine(const std::shared_ptr< CommandLine >& in_command_line)
{
	std::vector< std::shared_ptr< ILogConsumer >> array_consumer;

	{
		std::string file_path;
		if (true == in_command_line->GetValueString("log_file", file_path))
		{
			array_consumer.push_back(std::make_shared< LogConsumerWriteToFile >(file_path));
		}
	}
	if (true == in_command_line->GetFlag("log_console"))
	{
		array_consumer.push_back(std::make_shared< LogConsumerConsole>());
	}

	if ((true == in_command_line->GetFlag("log_output")) ||
		(((0 == array_consumer.size()) && (1 == in_command_line->GetParamCount()))))
	{
		array_consumer.push_back(std::make_shared< LogConsumerOutputDebugString >());
	}

	auto log = Log::Factory(array_consumer);
	return log;
}

std::shared_ptr< Log > Log::Factory(const LogContainer& in_array_consumer, const bool in_owner_singelton)
{
	auto pResult = std::make_shared< Log >(in_array_consumer, in_owner_singelton);
	return pResult;
}

Log::Log(const std::vector< std::shared_ptr< ILogConsumer >>& in_array_consumer, const bool in_owner_singelton)
	: _owner_singelton(in_owner_singelton)
{
	_Implementation = std::make_unique< LogImplementation >(in_array_consumer);
	if (true == _owner_singelton)
	{
		auto pOldSingelton = s_singleton.exchange(this);
		DSC_ASSERT(nullptr == pOldSingelton, "multiple log instances created");
		DrainLogMessages();
	}
}

Log::~Log()
{
	if (true == _owner_singelton)
	{
		auto pOldSingelton = s_singleton.exchange(nullptr);
		DSC_ASSERT(nullptr != pOldSingelton, "multiple log destruction");
	}
	_Implementation = nullptr;
}

//https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/8098080
void Log::AddMessage(const LogTopic in_topic, const char* const in_format, ... )
{
	auto log = s_singleton.load();
	// you know what, if the log doesn't exist, that may be intentional
	//DSC_ASSERT(pImple);
	bool dump_to_memory = false;
	if (nullptr == log)
	{
		dump_to_memory = true;
	}

	if ((nullptr != log) && (false == log->AcceptsTopic(in_topic)))
	{
		return;
	}

	va_list va_args;
	va_start(va_args, in_format);
	const std::string message = FormatString(in_format, va_args);
	va_end(va_args);

	if (false == dump_to_memory)
	{
		log->AddMessageInternal(in_topic, message);
	}
	else
	{
		s_CheckAllLogMessagesConsumed.AddMessage(in_topic, message);
		AddConsole("Warning: Message before log is ready: topic:[%d] message:[%s]", (int)in_topic, message.c_str());
		// we could log the fact that we are logging outside log scope? seems redundant though
		//if (topic != LogTopic::Log)
		//{
		//   AddMessage(LogTopic::Log, "Warning: topic:[%d] is trying to add messages to log outside log scope");
		//}
	}
}

void Log::RunningUintTest()
{
	s_CheckAllLogMessagesConsumed.RunningUintTest();
}

const bool Log::AcceptsTopic(const LogTopic in_topic)
{
	return _Implementation->AcceptsTopic(in_topic);
}

void Log::MemberAddMessage(const LogTopic in_topic, const char* const in_format, ... )
{
	if (false == _Implementation->AcceptsTopic(in_topic))
	{
		return;
	}

	va_list va_args;
	va_start(va_args, in_format);
	const std::string message = FormatString(in_format, va_args);
	va_end(va_args);

	_Implementation->AddMessage(in_topic, message);
}

void Log::DrainLogMessages()
{
	_Implementation->DrainLogMessages();
}

void Log::AddMessageInternal(const LogTopic in_topic, const std::string& in_message)
{
	_Implementation->AddMessage(in_topic, in_message);
}
