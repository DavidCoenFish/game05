#pragma once

#include <json/json.hpp>
class CommandLine;
class ITask;

class JSONTask
{
public:
	std::string _factory;
	nlohmann::json _data;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONTask,
	_factory,
	_data
	);

typedef std::function< std::shared_ptr< ITask >(
	const HINSTANCE in_instance, 
	const int in_cmd_show,
	const std::shared_ptr< CommandLine >& in_command_line, 
	const std::filesystem::path& in_path, 
	const nlohmann::json& in_json
	) > TTaskFactory;

class ITask
{
public:
	virtual ~ITask();

	virtual const int Run() = 0;

};
