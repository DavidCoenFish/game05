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
    /// [[_id,key2:type2,key3:type3,...],[data01,data02,data03,...],[data11,data12,data13,...] =>
    /// {data01:{key2:data02,key3:data03,...}, data11:{key2:data12,key2:data13,...},...}
    const bool Deal3rdObject(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const std::string& in_worksheet_name,
        const Cursor& in_cursor
        );

    /// [[key1:type1,key2:type2,key3:type3,...],[data01,data02,data03,...],[data11,data12,data13,...] =>
    /// [{key1:data01,key2:data02,key3:data03,...}, {key1:data11,key2:data12,key2:data13,...},...]
    const bool Deal3rdArray(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const std::string& in_worksheet_name,
        const Cursor& in_cursor
        );

    /// [[key1:type1,value1],[key2:type2,value2],...]    =>  {key1:value1, key2:value2}, 
    const bool Deal5th(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const std::string& in_worksheet_name,
        const Cursor& in_cursor
        );

    // how to do locale, and be able to only export one locale set (data set) at a time? 3rd normalise filters on header meta data
    // does Deal5thDataSet filte appropriate value?
    /// [[_id:string,dataset:<dataset1>,dataset:<dataset2>,...],[data00,data01,data02,...],[data10,data11,data12,...]]
    /// dataset==dataset1, {data00:data01,data10:data11,...}
    /// dataset==dataset2, {data00:data02,data10:data12,...}
    /// no dataset, {data00:{dataset1:data01,dataset2:data02,...},data10:{dataset1:data11,dataset2:data12,...} // this feels like 3rd from?
    /// is the token "_id" special and causes that data to be used as a key in the parent?

    // [[key1:_id, ?
}
