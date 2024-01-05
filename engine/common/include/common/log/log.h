#pragma once

#if defined(_DEBUG)
	#define DSC_LOG
#endif

#include "common/log/log_enum.h"

/*
keep on seeing a chicken and egg for who gets started first between file system and log
some logs may want to write to file using file system
some logs may want to flush on every write and not have any external dependencies
some logs may only want to write at application end of life?
some logs may want to append a log file
some logs may only want to save out file for messages of certain topic
some logs want to emit to stdout/console/push

move to run time constant set of consumers, and Log object/ having scope. 
the consumers may need to queue messages till they are ready, but may not need runtime flexibility
advantage of runtime constant is not needing to lock the thread of consumers on every message sent

what would a FileSystem log consumer look like, how does it know the provider filter? gets told when ready?
*/

#if defined(DSC_LOG)
	//print a message to the visual studio debug console imediatlly
	#define LOG_CONSOLE(format, ...) Log::AddConsole(format, ##__VA_ARGS__)

	//add message to log
	#define LOG_MESSAGE(format, ...) Log::AddMessage(LogTopic::None, format, ##__VA_ARGS__)
	#define LOG_MESSAGE_INFO(format, ...) Log::AddMessage(LogTopic::Info, format, ##__VA_ARGS__)
	#define LOG_MESSAGE_DEBUG(format, ...) Log::AddMessage(LogTopic::Debug, format, ##__VA_ARGS__)
	#define LOG_MESSAGE_WARNING(format, ...) Log::AddMessage(LogTopic::Warning, format, ##__VA_ARGS__)
	#define LOG_MESSAGE_ERROR(format, ...) Log::AddMessage(LogTopic::Error, format, ##__VA_ARGS__)
	#define LOG_MESSAGE_SCOPE(format, ...) Log::AddMessage(LogTopic::Scope, format, ##__VA_ARGS__)
	#define LOG_MESSAGE_FILESYSTEM(format, ...) Log::AddMessage(LogTopic::FileSystem, format, ##__VA_ARGS__)
	#define LOG_MESSAGE_UISYSTEM(format, ...) Log::AddMessage(LogTopic::UISystem, format, ##__VA_ARGS__)
	#define LOG_MESSAGE_RENDER(format, ...) Log::AddMessage(LogTopic::Render, format, ##__VA_ARGS__)

#else
	#define LOG_CONSOLE(format, ...) (void)0

	#define LOG_MESSAGE(format, ...) (void)0
    #define LOG_MESSAGE_INFO(format, ...) (void)0
    #define LOG_MESSAGE_DEBUG(format, ...) (void)0
    #define LOG_MESSAGE_WARNING(format, ...) (void)0
	#define LOG_MESSAGE_ERROR(format, ...) (void)0
	#define LOG_MESSAGE_SCOPE(format, ...) (void)0
	#define LOG_MESSAGE_FILESYSTEM(format, ...) (void)0
	#define LOG_MESSAGE_UISYSTEM(format, ...) (void)0
	#define LOG_MESSAGE_RENDER(format, ...) (void)0

#endif

#if defined(DSC_LOG) && 1
    // note: cmd line "log_output" will send log messages to visual studio output
	#define LOG_MESSAGE_UI_VERBOSE(format, ...) Log::AddMessage(LogTopic::UIVerbose, format, ##__VA_ARGS__)
	//#define LOG_MESSAGE_UI_VERBOSE(format, ...) Log::AddConsole(format, ##__VA_ARGS__)
#else
	#define LOG_MESSAGE_UI_VERBOSE(format, ...) (void)0
#endif

class CommandLine;
class ILogConsumer;
class LogImplimentation;

class Log
{
public:
	typedef std::vector< std::shared_ptr< ILogConsumer >> LogContainer;

	//any thread, no dependance on scope of Log object, just put a message on the visual studio output console
	static void AddConsole(const char* const in_format, ... );

	static std::shared_ptr< Log > FactoryCommandLine(const std::shared_ptr< CommandLine >& in_command_line);

	//for the application, only call once with bOwnerSingelton==true, singelton is used for static AddMessage as used by the macros LOG_MESSAGE...
	// bOwnerSingelton==false is used for unit test to check the Log object works
	static std::shared_ptr< Log > Factory(const LogContainer& in_array_consumer, const bool in_owner_singelton = true);

	Log(const std::vector< std::shared_ptr< ILogConsumer >>& in_array_consumer, const bool in_owner_singelton);
	~Log();

	//any thread, asserts if outside scope of Log object
	static void AddMessage(const LogTopic in_topic, const char* const in_format, ... );

	//let logs know that unit test are running and not to panic if messages are not consumed
	static void RunningUintTest();

	//for unit test to directly add message to Log object
	void MemberAddMessage(const LogTopic in_topic, const char* const in_format, ... );

private:
	const bool AcceptsTopic(const LogTopic in_topic);
	void DrainLogMessages();

	void AddMessageInternal(const LogTopic in_topic, const std::string& in_message);

private:
	bool _owner_singelton;
	std::unique_ptr< LogImplimentation > _implimentation;

};
