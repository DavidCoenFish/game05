#include "string_to_worksheet.h"
#include "worksheet.h"

#include <pugixml.hpp>

namespace
{
    const char* GetAttribute(const pugi::xml_node& in_node, const char* in_key)
    {
        return in_node.attribute(in_key).as_string("");
    }
    const char* GetParentAttribute(const pugi::xml_node& in_node, const char* in_key)
    {
        return in_node.parent().attribute(in_key).as_string("");
    }

    const char* GetChildText(const pugi::xml_node& in_node)
    {
        return in_node.first_child().text().get();
    }

    typedef std::map<std::string, std::shared_ptr<Worksheet>> TWorksheetMap;
    class SimpleWalker: public pugi::xml_tree_walker
    {
    public:
        SimpleWalker(TWorksheetMap& out_worksheet_map)
            : _worksheet_map(out_worksheet_map)
        {
            // nop
        }

        ~SimpleWalker()
        {
            // append any remaining data to the back of the last worksheet
            StartNewRow();
        }

    private:
        void StartNewRow()
        {
            if ((nullptr != _current_worksheet) && (true == _current_row_has_valid_data))
            {
                _current_worksheet->AppendRow(_current_row);
            }

            _current_row.clear();
            _current_row_has_valid_data = false;
        }

        virtual bool for_each(pugi::xml_node& in_node)
        {
            const char* parent_id = GetParentAttribute(in_node, "id");
            const char* id = GetAttribute(in_node, "id");

            //<div id="doc-title"><span class="name">character</span>
            if (0 == strcmp(id, "doc-title"))
            {
                const char* name = GetChildText(in_node);
                printf("Document name:%s\n", name);
            }

            //following the observed source of a google sheets published sheet, all the sheets have a button to link to them
            if (0 == strncmp(id, "sheet-button-", 13))
            {
                const char* name = GetChildText(in_node);
                //printf("Sheet name:%s\n", name);
                _sheet_names.push_back(name);
            }

            //then there is the display area with all the sheet data "sheets-viewport" with each sheet in ture defined as a table
            if (0 == strcmp(parent_id, "sheets-viewport"))
            {
                _sheet_index += 1;
                if ((0 <= _sheet_index) && (_sheet_index < static_cast<int>(_sheet_names.size())))
                {
                    StartNewRow();

                    _current_worksheet = std::make_shared<Worksheet>();
                    const std::string name = _sheet_names[_sheet_index];
                    _worksheet_map[name] = _current_worksheet;

                    printf("    parse worksheet:%s\n", name.c_str());
                }
            }

            // for each row in the table
            if (0 == strcmp(in_node.name(), "tr"))
            {
                StartNewRow();
            }

            // ignore "th", the row index is displayed, but that is not relevant data for us, so ignored

            // the table data row. to preserve order, need to add empty cells, but if all the cells in a row are empty, ignore the row, bool _current_row_has_valid_data
            if (0 == strcmp(in_node.name(), "td"))
            {
                const std::string value = GetChildText(in_node);
                if (nullptr != _current_worksheet)
                {
                    _current_row_has_valid_data |= (false == value.empty());
                    _current_row.push_back(value);
                }
            }

            return true; // continue traversal
        }

    private:
        TWorksheetMap& _worksheet_map;

        // temp data for loading state
        std::vector<std::string> _sheet_names = {};
        int _sheet_index = -1;

        std::shared_ptr<Worksheet> _current_worksheet;
        std::vector<std::string> _current_row;
        bool _current_row_has_valid_data;
    };

}

const bool StringToWorksheet::DealSourceData(std::map<std::string, std::shared_ptr<Worksheet>>& out_source_data, const std::string& in_source_data)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(in_source_data.c_str());

    if (pugi::status_ok != result.status)
    {
        return false;
    }

    {
        SimpleWalker simple_walker(out_source_data);
        doc.traverse(simple_walker);
    }

    return true;
}
