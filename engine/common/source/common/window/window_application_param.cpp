#include "common/common_pch.h"
#include "common/window/window_application_param.h"
#include "common/util/command_line.h"

#include <json/json.hpp>

WindowApplicationParam::WindowApplicationParam(
    const bool in_full_screen,
    const int in_width,
    const int in_height,
    const std::shared_ptr< CommandLine >& in_command_line,
    const std::filesystem::path& in_root_path, // Where the exe is
    const std::filesystem::path& in_data_path, // Where application.json is
    const nlohmann::json& in_json,
    TaskWindow* const in_task_window
    )
    : _full_screen(in_full_screen)
    , _width(in_width)
    , _height(in_height)
    , _command_line(in_command_line)
    , _root_path(in_root_path)
    , _data_path(in_data_path)
    , _json(in_json)
    , _task_window(in_task_window)
{
    //nop
}
