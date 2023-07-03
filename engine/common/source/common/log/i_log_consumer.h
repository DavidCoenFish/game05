#pragma once

#include "common/log/log_enum.h"

class ILogConsumer
{
public:
   virtual ~ILogConsumer();

   virtual void AddMessage(const LogTopic in_topic, const std::string& in_message ) = 0;

   virtual const bool AcceptsTopic(const LogTopic in_topic) = 0;

   // don't support run time change of supported topics, reasons. performance and thread safty
   //virtual void RegisterAcceptsTopicChangeCallback(const std::function<void(void)>& callback) = 0;
};
