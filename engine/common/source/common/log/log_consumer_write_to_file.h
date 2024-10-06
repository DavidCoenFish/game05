#pragma once
#include "common/log/i_log_consumer.h"

//writes to disk, no dependency on FileSystem class
class LogConsumerWriteToFile: public ILogConsumer
{
public:
   LogConsumerWriteToFile(
	  const std::string& in_log_file_path = GetDefaultPath(),
	  const std::vector<bool>& in_topic_filter_array_or_empty = std::vector<bool>()
   );
   ~LogConsumerWriteToFile();
private:
   static const std::string GetDefaultPath();
   virtual void AddMessage(const LogTopic in_topic, const std::string& in_message ) override;
   virtual const bool AcceptsTopic(const LogTopic in_topic) override;

private:
   std::vector<bool> _topic_filter_array_or_empty;

   std::mutex _file_lock;
   std::ofstream _file;
};
