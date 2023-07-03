#pragma once
#include "common/log/i_log_consumer.h"

//writes to disk, no dependency on FileSystem class
class LogConsumerOutputDebugString: public ILogConsumer
{
public:
	LogConsumerOutputDebugString(const std::vector<bool>& in_topic_filter_array_or_empty = std::vector<bool>());
	~LogConsumerOutputDebugString();

private:
	virtual void AddMessage(const LogTopic in_topic, const std::string& in_message ) override;
	virtual const bool AcceptsTopic(const LogTopic in_topic) override;

private:
	std::vector<bool> _topic_filter_array_or_empty;

};
