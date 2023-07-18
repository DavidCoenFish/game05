#include "common/common_pch.h"

#include "common/task/task_window.h"
#include "common/window/window_helper.h"
#include "common/window/window_application_param.h"
#include "common/window/i_window_application.h"

#include <json/json.hpp>

class JSONWindow
{
public:
	JSONWindow()
		: _full_screen(false)
		, _width(800)
		, _height(600)
	{
		//nop
	}
public:
    std::string _name;
	bool _full_screen;
	int _width;
	int _height;
	std::string _factory;
	nlohmann::json _data;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONWindow,
	_name,
	_full_screen,
	_width,
	_height,
	_factory,
	_data
	);


const TWindowApplicationFactory GetApplicationFactory(
    const std::string& in_factory_name,
    const std::map<std::string, TWindowApplicationFactory>& in_application_factory
    )
{
	const auto found = in_application_factory.find(in_factory_name);
	if (found != in_application_factory.end())
	{
		return found->second;
	}
	return [](const HWND in_hwnd, const WindowApplicationParam& in_param)
	{
		return new IWindowApplication(in_hwnd, in_param);
	};
}

const std::shared_ptr<TaskWindow> TaskWindow::Factory(
	const HINSTANCE in_instance,
	const int in_cmd_show,
	const std::shared_ptr< CommandLine >& in_command_line,
	const std::filesystem::path& in_path,
	const nlohmann::json& in_json,
    const std::map<std::string, TWindowApplicationFactory>& in_application_factory
    )
{
	std::vector<JSONWindow> array_window;
	in_json.get_to(array_window);

	auto result = std::make_shared<TaskWindow>();
	TaskWindow* raw_task_window = result.get();
	for (const auto& item : array_window)
	{
		auto application_param = WindowApplicationParam(
			item._full_screen,
			item._width,
			item._height,
			in_command_line,
			in_path,
			item._data,
			raw_task_window
			);

		auto application_factory = GetApplicationFactory(item._factory, in_application_factory);
		auto application_factory_wrapped = [raw_task_window, application_factory](const HWND in_hwnd, const WindowApplicationParam& in_param)
		{
			IWindowApplication* application = application_factory(in_hwnd, in_param);
			std::shared_ptr< IWindowApplication > application_shared(application);
			raw_task_window->_application_list.push_back(application_shared);
			return application;
		};

		WindowHelper(
			application_factory_wrapped,
			application_param,
			in_instance,
			item._name,
			in_cmd_show
			);
	}

	return result;
}

TaskWindow::TaskWindow()
	: _continue(true)
{
	return;
}

TaskWindow::~TaskWindow()
{
	return;
}

const int TaskWindow::Run()
{
	//while we have windows, keep pushing messages
	MSG msg = {};
	int exitCode = 0;
	while (true == _continue)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
			{
				exitCode = (int)msg.wParam;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			for (auto iter: _application_list)
			{
				iter->Update();
			}
		}
	}

	return exitCode;
}

void TaskWindow::DestroyApplication(IWindowApplication* in_application, const int in_exit_code)
{
	_application_list.erase(
		std::remove_if(_application_list.begin(), _application_list.end(), [=](std::shared_ptr< IWindowApplication >& item){
		return (in_application == item.get());
		}),
		_application_list.end());
	if (0 == _application_list.size())
	{
		PostQuitMessage(in_exit_code);
	}
	return;
}
