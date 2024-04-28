#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include <json\json.hpp>

class Worksheet;

class State
{
public:
    bool do_help = false;

    std::string input_file_path = {};
    std::string input_url = {};

    std::map<std::string, std::shared_ptr<Worksheet>> source_data;

    std::string sheet_3rd_worksheet_name = {};
    std::string sheet_5th_worksheet_name = {};
    std::string sheet_3rd_key_value_worksheet_name = {};

    std::string output_file_path = {};

    bool do_data_set_filter = false;
    /// filter values via items given in data set
    std::set<std::string> data_set = {};

    nlohmann::json output_data = nlohmann::json({});
};
