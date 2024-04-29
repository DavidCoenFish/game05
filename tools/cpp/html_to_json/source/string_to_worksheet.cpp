#include "string_to_worksheet.h"
#include "worksheet.h"

#include <memory>
#include <map>
#include <string>
#include <pugixml.hpp>

namespace
{
    typedef std::map<std::string, std::shared_ptr<Worksheet>> TWorksheetMap;
    class SimpleWalker: public pugi::xml_tree_walker
    {
    public:
        SimpleWalker(TWorksheetMap& out_worksheet_map)
            : _worksheet_map(out_worksheet_map)
        {
            // nop
        }

        virtual bool for_each(pugi::xml_node& node)
        {
            //printf("%s\n", node.name());

                //pugi::xml_attribute attribute = node.attribute("d");

            return true; // continue traversal
        }

    private:
        TWorksheetMap& _worksheet_map;

    };

}

const bool StringToWorksheet::DealSourceData(std::map<std::string, std::shared_ptr<Worksheet>>& out_source_data, const std::string& in_source_data)
{
    // xml_parse_result xml_document::load_string
    pugi::xml_document doc;

    //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    //std::string narrow = converter.to_bytes(wide_utf16_source_string);
    //std::wstring wide = converter.from_bytes(in_source_data);

    pugi::xml_parse_result result = doc.load_string(in_source_data.c_str());

    if (pugi::status_ok != result.status)
    {
        return false;
    }

    SimpleWalker simple_walker(out_source_data);
    doc.traverse(simple_walker);

    return true;
}
