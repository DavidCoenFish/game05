#pragma once

#include "common/task/i_task.h"
class IWindowApplication;

class TaskWindow : public ITask
{
public:
	static const std::shared_ptr<TaskWindow> Factory(
		const HINSTANCE in_instance,
		const int in_cmd_show,
		const std::shared_ptr< CommandLine >& in_command_line,
		const std::filesystem::path& in_root_path,
		const std::filesystem::path& in_data_path,
		const nlohmann::json& in_json,
		const std::map<std::string, TWindowApplicationFactory>& in_application_factory
		);

	TaskWindow();
	virtual ~TaskWindow();

	virtual const int Run() override;

	void DestroyApplication(
		IWindowApplication* in_application, 
		const int in_exit_code
		);

private:
	std::vector<std::shared_ptr<IWindowApplication>> _application_list;
	bool _continue;

};
