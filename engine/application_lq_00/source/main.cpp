#include "application_lq_00_pch.h"

#include "build.h"

#include "common/log/log.h"
#include "common/file_system/file_system.h"
#include "common/task/i_task.h"
#include "common/task/task_window.h"
#include "common/util/command_line.h"
#include "common/util/utf8.h"

#include "window_application/application_demo_00.h"

#include <json/json.hpp>

struct JSONApplication
{
    std::vector< JSONTask > _tasks;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE
(
    JSONApplication, 
    _tasks
);

static std::map< std::string, TTaskFactory >& GetTaskFactoryMap()
{
    static std::map< std::string, TTaskFactory > s_map(
    {
        {"Window", TaskWindow::Factory}
    });
    return s_map;
}

static std::map< std::string, TWindowApplicationFactory >& GetWindowApplicationFactoryMap()
{
    static std::map< std::string, TWindowApplicationFactory > s_map(
        {
            {"demo_00", ApplicationDemo00::Factory},
        });
    return s_map;
}

static const TTaskFactory GetTaskFactory(const std::string& in_task_factory_key)
{
    const auto& factoryMap = GetTaskFactoryMap();
    auto found = factoryMap.find(in_task_factory_key);
    if (found != factoryMap.end())
    {
        return found->second;
    }
    return nullptr;
}

static const int RunTask(HINSTANCE in_instance, int in_cmd_show)
{
    auto command_line = CommandLine::Factory(Utf8::Utf16ToUtf8(GetCommandLineW()));
    if (nullptr == command_line)
    {
        return -1;
    }

    auto log = Log::FactoryCommandLine(command_line);

    LOG_MESSAGE("Build %s %s %s %s %s", Build::GetBuildVersion(), Build::GetBuildTime(), Build::GetBuildHost(), Build::GetBuildConfiguration(), Build::GetBuildPlatform());

    int result = 0;
    std::string task_name("empty");

    if (2 <= command_line->GetParamCount())
    {
        task_name = command_line->GetParam(1);
    }
    else
    {
#if defined(_DEBUG)
        LOG_MESSAGE_ERROR("Only got [%d] param, want at least a task name", command_line->GetParamCount());
        return -1;
#else
        task_name = "basic_ui";
#endif
    }

    {
        std::filesystem::path root_path = FileSystem::GetModualDir(in_instance);
        std::filesystem::path data_path = root_path / "task" / task_name;
        std::filesystem::path application_json_path = data_path / "application.json";
        auto file_string = FileSystem::DataToString(FileSystem::SyncReadFile(application_json_path));
        auto json = nlohmann::json::parse(file_string);
        JSONApplication application_data;
        json.get_to(application_data);
        auto window_application_factory_map = GetWindowApplicationFactoryMap();

        for (const auto& item: application_data._tasks)
        {
            auto task_factory = GetTaskFactory(item._factory);
            auto task = (nullptr != task_factory) ? task_factory(
                in_instance, 
                in_cmd_show, 
                command_line, 
                root_path,
                data_path,
                item._data,
                window_application_factory_map
                ) : nullptr;
            if (task)
            {
                result = task->Run();
            }
            if (result < 0)
            {
                LOG_MESSAGE_ERROR("task returner [%d], abort run", result);
            }
        }
    }

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
