#pragma once

#include <memory>
#include <map>
#include <set>
#include <string>
#include <json\json.hpp>

class Cursor;
class Worksheet;

namespace WorksheetToJson
{
    const bool Deal3rd(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const std::string& in_worksheet_name,
        const Cursor& in_cursor
        );

    const bool Deal3rdArray(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const std::string& in_worksheet_name,
        const Cursor& in_cursor
        );

    const bool Deal5th(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const std::string& in_worksheet_name,
        const Cursor& in_cursor
        );
}
