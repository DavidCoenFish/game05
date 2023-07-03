#include "common/common_pch.h"

#include "common/log/log_consumer_write_to_file.h"
#include "common/log/log.h"

LogConsumerWriteToFile::LogConsumerWriteToFile(
	const std::string& in_log_file_path,
	const std::vector<bool>& in_topic_filter_array_or_empty
	)
	: _topic_filter_array_or_empty(in_topic_filter_array_or_empty)
{
	std::lock_guard< std::mutex > lock(_file_lock);
	_file.open(in_log_file_path.c_str(), std::ofstream::out);
}

LogConsumerWriteToFile::~LogConsumerWriteToFile()
{
	std::lock_guard< std::mutex > lock(_file_lock);
	if (true == _file.is_open())
	{
		_file.flush();
		_file.close();
	}
}

const std::string LogConsumerWriteToFile::GetDefaultPath()
{
	return "log.txt";
}

void LogConsumerWriteToFile::AddMessage(const LogTopic topic, const std::string& message )
{
	std::lock_guard< std::mutex > lock(_file_lock);
	if (true == _file.is_open())
	{
		//warning, does "<< std::endl" flush, and is that ok?
		_file << (int)topic << ":" << message.c_str() << std::endl;
	}
}

const bool LogConsumerWriteToFile::AcceptsTopic(const LogTopic topic)
{
	if ((0 <= (int)topic) && ((int)topic < _topic_filter_array_or_empty.size()))
	{
		return _topic_filter_array_or_empty[(int)topic];
	}

	return true;
}

