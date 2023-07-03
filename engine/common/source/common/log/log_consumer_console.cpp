#include "common/common_pch.h"

#include "common/log/log_consumer_console.h"
#include "common/log/log.h"
#include "common/util/utf8.h"

LogConsumerConsole::LogConsumerConsole(const std::vector<bool>& in_topic_filter_array_or_empty)
	: _topic_filter_array_or_empty(in_topic_filter_array_or_empty)
{
	if (TRUE == AttachConsole(ATTACH_PARENT_PROCESS))
	{
		FILE* pFileOut = nullptr;
		freopen_s(&pFileOut, "conout$","w",stdout);
		LOG_MESSAGE("AttachConsole");
	}
}

LogConsumerConsole::~LogConsumerConsole()
{
	//nop
}

void LogConsumerConsole::AddMessage(const LogTopic in_topic, const std::string& in_message )
{
	std::string text = std::to_string((int)in_topic) + std::string(":") + in_message + "\n";
	//OutputDebugStringW(Utf8::Utf8ToUtf16(text).c_str());
	printf(text.c_str());
}

const bool LogConsumerConsole::AcceptsTopic(const LogTopic in_topic)
{
	if ((0 <= (int)in_topic) && ((int)in_topic < _topic_filter_array_or_empty.size()))
	{
		return _topic_filter_array_or_empty[(int)in_topic];
	}

	return true;
}

