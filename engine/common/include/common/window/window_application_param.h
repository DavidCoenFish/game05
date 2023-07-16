#pragma once

#include <json/json.hpp>

class TaskWindow;
class CommandLine;

class WindowApplicationParam
{
public:
	WindowApplicationParam(
		const bool in_full_screen,
		const int in_width,
		const int in_height,
		const std::shared_ptr< CommandLine >& in_command_line,
		const std::filesystem::path& in_root_path,
		const nlohmann::json& in_json,
		TaskWindow* const in_task_window
	);
	const bool  _full_screen;
	const int  _width;
	const int  _height;
	const std::shared_ptr< CommandLine > _command_line;
	const std::filesystem::path _root_path;
	const nlohmann::json _json;
	TaskWindow* const _task_window;
};
