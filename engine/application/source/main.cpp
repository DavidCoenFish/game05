//
// Main.cpp
//

#include "application_pch.h"
#include "build.h"
#include "common/log/log.h"
#include "common/util/command_line.h"
#include "common/util/utf8.h"
//#include "json/json.hpp"

static const int RunTask(HINSTANCE in_instance, int in_cmd_show)
{
	in_instance;
	in_cmd_show;

	auto command_line = CommandLine::Factory(Utf8::Utf16ToUtf8(GetCommandLineW()));
	if (nullptr == command_line)
	{
		return -1;
	}

	auto log = Log::FactoryCommandLine(command_line);

	LOG_MESSAGE("Build %s %s %s", Build::GetBuildVersion(), Build::GetBuildTime(), Build::GetBuildDescription());


	int result = 0;
	std::string task_name("Empty");

	if (2 <= command_line->GetParamCount())
	{
		task_name = command_line->GetParam(1);
	}
	else
	{
		LOG_MESSAGE_ERROR("Only got [%d] param, want at least a task name", command_line->GetParamCount());
	}

	//{
	//   std::filesystem::path path = FileSystem::GetModualDir(hInstance) / "Task" / taskName;
	//   std::filesystem::path applicationPath = path / "Application.json";
	//   auto fileString = FileSystem::DataToString(FileSystem::SyncReadFile(applicationPath));
	//   auto json = nlohmann::json::parse( fileString );
	//   JSONApplication applicationData;
	//   json.get_to(applicationData);

	//   for (const auto& item: applicationData.tasks)
	//   {
	//      auto taskFactory = GetTaskFactory(item.factory);
	//      auto pTask = (nullptr != taskFactory) ? taskFactory(hInstance, nCmdShow, pCommandLine, path, item.data) : nullptr;
	//      if (pTask)
	//      {
	//         result = pTask->Run();
	//      }
	//      if (result < 0)
	//      {
	//         LOG_MESSAGE_ERROR("task returner [%d], abort run", result);
	//      }
	//   }
	//}

	return result;
}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE in_instance, _In_opt_ HINSTANCE in_prev_instance, _In_ LPWSTR in_cmd_line, _In_ int in_cmd_show)
{
	UNREFERENCED_PARAMETER(in_prev_instance);
	UNREFERENCED_PARAMETER(in_cmd_line);

	const int result = RunTask(in_instance, in_cmd_show);

	return result;
}
