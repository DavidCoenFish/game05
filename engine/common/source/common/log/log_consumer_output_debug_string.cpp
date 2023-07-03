#include "common/common_pch.h"

#include "common/log/log_consumer_output_debug_string.h"
#include "common/log/log.h"
#include "common/util/utf8.h"

LogConsumerOutputDebugString::LogConsumerOutputDebugString(
	const std::vector<bool>& in_topic_filter_array_or_empty
	)
	: _topic_filter_array_or_empty(in_topic_filter_array_or_empty)
{
	// nop
}

LogConsumerOutputDebugString::~LogConsumerOutputDebugString()
{
	// nop
}

void LogConsumerOutputDebugString::AddMessage(const LogTopic in_topic, const std::string& in_message )
{
	std::string text = std::to_string((int)in_topic) + std::string(":") + in_message + "\n";
	OutputDebugStringW(Utf8::Utf8ToUtf16(text).c_str());
}

const bool LogConsumerOutputDebugString::AcceptsTopic(const LogTopic topic)
{
	if ((0 <= (int)topic) && ((int)topic < _topic_filter_array_or_empty.size()))
	{
		return _topic_filter_array_or_empty[(int)topic];
	}

	return true;
}

