#pragma once
#include "common/log/i_log_consumer.h"

class LogConsumerConsole : public ILogConsumer
{
public:
	LogConsumerConsole(const std::vector<bool>& in_topic_filter_array_or_empty = std::vector<bool>());
	~LogConsumerConsole();

private:
	virtual void AddMessage(const LogTopic in_topic, const std::string& in_message ) override;
	virtual const bool AcceptsTopic(const LogTopic in_topic) override;

private:
	std::vector<bool> _topic_filter_array_or_empty;

};
