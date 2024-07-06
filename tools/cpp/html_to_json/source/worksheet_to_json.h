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
    /// [[_id:string, 

    /// [[_id,key2:type2,key3:type3,...],[data01,data02,data03,...],[data11,data12,data13,...] =>
    /// {data01:{key2:data02,key3:data03,...}, data11:{key2:data12,key2:data13,...},...}
    const bool Deal3rd(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const std::string& in_worksheet_name,
        const Cursor& in_cursor
        );

    /// [[key1:typeA,valueA],[key2:typeB,valueB],...]    =>  {key1:valueA, key2:valueB}, 
    const bool Deal5th(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const std::string& in_worksheet_name,
        const Cursor& in_cursor
        );

    /// how to do locale, and be able to only export one locale set (data set) at a time? 3rd normalise filters on header meta data
    /// this is set up like 3rd form with 5th form output, empty or first if no dataset?
    /// [[_id:string,dataset:<dataset1>,dataset:<dataset2>,...],[data00,data01,data02,...],[data10,data11,data12,...]]
    /// dataset==dataset1, {data00:data01,data10:data11,...}
    /// dataset==dataset2, {data00:data02,data10:data12,...}
    const bool Deal3rdKeyValue(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const std::string& in_worksheet_name,
        const Cursor& in_cursor
        );

}
