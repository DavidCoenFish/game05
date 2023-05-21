#pragma once

#if defined(_DEBUG)
   #define DSC_LOG
#endif

enum class LogTopic : int
{
   None = 0,
   Debug,
   Warning,
   Error,

   Count
};

#if defined(DSC_LOG)
   //print a message to the visual studio debug console imediatlly
   #define LOG_CONSOLE(format, ...) Log::AddConsole(format, ##__VA_ARGS__)

   //add message to log
   #define LOG_MESSAGE(format, ...) Log::AddMessage(LogTopic::None, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_DEBUG(format, ...) Log::AddMessage(LogTopic::Debug, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_WARNING(format, ...) Log::AddMessage(LogTopic::Warning, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_ERROR(format, ...) Log::AddMessage(LogTopic::Error, format, ##__VA_ARGS__)
#else
   #define LOG_CONSOLE(format, ...) (void)0
   #define LOG_MESSAGE(format, ...) (void)0
   #define LOG_MESSAGE_DEBUG(format, ...) (void)0
   #define LOG_MESSAGE_WARNING(format, ...) (void)0
   #define LOG_MESSAGE_ERROR(format, ...) (void)0
#endif

namespace Log
{
   //any thread, asserts if outside scope of Log object
   void AddMessage(const LogTopic topic, const char* const pFormat, ... );
}
