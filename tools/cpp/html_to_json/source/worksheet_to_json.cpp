#include "worksheet_to_json.h"

#include "cursor.h"
#include "worksheet.h"

namespace
{
    enum class ActionEnum
    {
        TIgnore,
        TInt,
        TFloat,
        TBool,
        TString,
        TSheet3rd, //make a map, obliges an _id string to use as map key
        TSheet5th,
        TSheet3rdKeyValue
    };

    void SplitString(
        std::vector<std::string>& out_split, 
        const std::string& in_string, 
        const char in_split)
    {
        out_split.clear();
        out_split.push_back("");
        for (auto iter : in_string)
        {
            if (iter == in_split)
            {
                out_split.push_back("");
            }
            else
            {
                out_split.back() += iter;
            }
        }
        return;
    }

    const bool Contains(
        const std::vector<std::string>& in_data, 
        const std::string& in_needle
        )
    {
        for (const auto iter : in_data)
        {
            if (iter == in_needle)
            {
                return true;
            }
        }
        return false;
    }

    const int StringToInt(const std::string& in_string)
    {
        return std::stoi(in_string);
    }
    const float StringToFloat(const std::string& in_string)
    {
        return std::stof(in_string);
    }
    const bool StringToBoolean(const std::string& in_string)
    {
        if ((0 == strcmp("TRUE", in_string.c_str())) ||
            (0 == strcmp("true", in_string.c_str())) ||
            (0 == strcmp("True", in_string.c_str())) ||
            (0 == strcmp("1", in_string.c_str())))
        {
            return true;
        }
        return false;
    }

    //https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
    const bool IsInteger(const std::string& in_string)
    {
        return !in_string.empty() && 
            std::find_if(
                in_string.begin(), 
                in_string.end(), 
                [](unsigned char c) { return !std::isdigit(c); }
                ) == in_string.end();
    }

    const ActionEnum DealKey(
        nlohmann::json& in_out_data,
        Cursor& in_out_cursor,
        const std::string& in_key
        )
    {
        bool name_set = false;
        ActionEnum action = ActionEnum::TIgnore;

        std::vector<std::string> split_key;
        SplitString(split_key, in_key, ':');

        const int count = static_cast<int>(split_key.size());
        for (int index = 0; index < count; ++index)
        {
            const std::string& token = split_key[index];
            if (0 == strcmp("ignore", token.c_str()))
            {
                return ActionEnum::TIgnore;
            }
            else if (0 == strcmp("int", token.c_str()))
            {
                action = ActionEnum::TInt;
            }
            else if (0 == strcmp("float", token.c_str()))
            {
                action = ActionEnum::TFloat;
            }
            else if (0 == strcmp("bool", token.c_str()))
            {
                action = ActionEnum::TBool;
            }
            else if (0 == strcmp("string", token.c_str()))
            {
                action = ActionEnum::TString;
            }
            else if (0 == strcmp("sheet3rd", token.c_str()))
            {
                action = ActionEnum::TSheet3rd;
            }
            //else if (0 == strcmp("sheet3rdArray", token.c_str()))
            //{
            //    action = ActionEnum::TSheet3rdArray;
            //}
            else if (0 == strcmp("sheet3rdKeyValue", token.c_str()))
            {
                action = ActionEnum::TSheet3rdKeyValue;
            }
            else if (0 == strcmp("sheet5th", token.c_str()))
            {
                action = ActionEnum::TSheet5th;
            }
            else if (0 == strcmp("array", token.c_str()))
            {
                if (false == name_set)
                {
                    printf("need name before array in key:%s\n", in_key.c_str());
                }
                int next_index = index + 1;
                bool added = false;
                if (next_index < count)
                {
                    const std::string& next_token = split_key[next_index];
                    if (true == IsInteger(next_token))
                    {
                        in_out_cursor.PushArray(StringToInt(next_token));
                        index = next_index;
                        added = true;
                    }
                }
                if (false == added)
                {
                    const int sub_index = in_out_cursor.GetArrayLength(in_out_data);
                    in_out_cursor.PushArray(sub_index);
                }
            }
            else if (0 == strcmp("dataset", token.c_str()))
            {
                // consume all the dataset token, if none match, ignore
                bool data_set_pass = false;
                //dataset:en:string
                while (index < count)
                {
                    if (true == in_out_cursor.TestDataset(split_key[index + 1]))
                    {
                        data_set_pass = true;
                    }
                    index += 1;
                    if (0 != strcmp("dataset", split_key[index + 1].c_str()))
                    {
                        break;
                    }
                }
                if (false == data_set_pass)
                {
                    return ActionEnum::TIgnore;
                }
            }
            else
            {
                in_out_cursor.PushMember(token);
                name_set = true;
            }
        }

        //if (ActionEnum:TIgnore == action)
        //if ((true !== in_ignoreMissingName) && (false == name_set))

        return action;
    }

    void ConsumeCell(
        nlohmann::json& out_data,
        const Cursor& in_cursor,
        const ActionEnum in_action, 
        const std::string& in_cell,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets
        )
    {
        if (true == in_cell.empty())
        {
            return;
        }
        switch(in_action)
        {
        default:
            break;
        case ActionEnum::TInt:
            in_cursor.SetValue(out_data, nlohmann::json(StringToInt(in_cell)));
            break;
        case ActionEnum::TFloat:
            in_cursor.SetValue(out_data, nlohmann::json(StringToFloat(in_cell)));
            break;
        case ActionEnum::TBool:
            in_cursor.SetValue(out_data, nlohmann::json(StringToBoolean(in_cell)));
            break;
        case ActionEnum::TString:
            in_cursor.SetValue(out_data, nlohmann::json(in_cell));
            break;
        case ActionEnum::TSheet3rd:
            WorksheetToJson::Deal3rd(out_data, in_worksheets, in_cell, in_cursor.CloneSetUseDataSet(false));
            break;
        case ActionEnum::TSheet5th:
            WorksheetToJson::Deal5th(out_data, in_worksheets, in_cell, in_cursor.CloneSetUseDataSet(false));
            break;
        case ActionEnum::TSheet3rdKeyValue:
            WorksheetToJson::Deal3rdKeyValue(out_data, in_worksheets, in_cell, in_cursor.CloneSetUseDataSet(true));
            break;
        }
    }

    void Sheet3rdNormalisedRow(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const Cursor& in_cursor,
        const std::vector<std::string>& in_key_row,
        const Worksheet& in_worksheet, 
        const int in_row_index
        )
    {
        const int cell_count = in_worksheet.GetRowCellCount(in_row_index);
        const int count = std::min(cell_count, static_cast<int>(in_key_row.size()));
        if (count <= 0)
        {
            return;
        }
        const std::string id = in_worksheet.GetCell(in_row_index, 0);

        // skip blank id~ comment row?
        if (true == id.empty())
        {
            return;
        }

        auto local_cursor = in_cursor.Clone();
        local_cursor.PushMember(id);
        local_cursor.SetValue(out_data, nlohmann::json(nlohmann::json::value_t::object));

        for (int index = 1; index < count; ++index)
        {
            auto local_cursor_inner = local_cursor.Clone();
            const ActionEnum action = DealKey(
                out_data, 
                local_cursor_inner,
                in_key_row[index]
                );
            if (ActionEnum::TIgnore == action)
            {
                continue;
            }

            const std::string cell = in_worksheet.GetCell(in_row_index, index);
            ConsumeCell(
                out_data, 
                local_cursor_inner, 
                action, 
                cell,
                in_worksheets
                );
        }
        return;
    }

    void Sheet3rdNormalisedRowKeyValue(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const Cursor& in_cursor,
        const std::vector<std::string>& in_key_row,
        const Worksheet& in_worksheet, 
        const int in_row_index
        )
    {
        const int cell_count = in_worksheet.GetRowCellCount(in_row_index);
        const int count = std::min(cell_count, static_cast<int>(in_key_row.size()));
        if (count <= 0)
        {
            return;
        }
        const std::string id = in_worksheet.GetCell(in_row_index, 0);

        // skip blank id~ comment row?
        if (true == id.empty())
        {
            return;
        }

        auto local_cursor = in_cursor.Clone();
        local_cursor.PushMember(id);
        //local_cursor.SetValue(out_data, nlohmann::json(nlohmann::json::value_t::object));

        for (int index = 1; index < count; ++index)
        {
            auto local_cursor_inner = local_cursor.Clone();
            const ActionEnum action = DealKey(
                out_data, 
                local_cursor_inner,
                in_key_row[index]
                );
            if (ActionEnum::TIgnore == action)
            {
                continue;
            }

            const std::string cell = in_worksheet.GetCell(in_row_index, index);
            ConsumeCell(
                out_data, 
                local_cursor_inner, 
                action, 
                cell,
                in_worksheets
                );
            break;
        }
        return;
    }

    void Sheet5thNormalisedRow(
        nlohmann::json& out_data,
        const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
        const Cursor& in_cursor,
        const Worksheet& in_worksheet, 
        const int in_row_index
        )
    {
        const int cell_count = in_worksheet.GetRowCellCount(in_row_index);
        if (cell_count <= 0)
        {
            return;
        }
        const std::string key = in_worksheet.GetCell(in_row_index, 0);

        auto local_cursor = in_cursor.Clone();
        const ActionEnum action = DealKey(
            out_data, 
            local_cursor,
            key
            );
        if (ActionEnum::TIgnore == action)
        {
            return;
        }

        if (2 <= cell_count)
        {
            const std::string cell = in_worksheet.GetCell(in_row_index, 1);
            ConsumeCell(
                out_data, 
                local_cursor, 
                action, 
                cell,
                in_worksheets
                );
        }
        return;
    }

} // end namespace

const bool WorksheetToJson::Deal3rd(
    nlohmann::json& out_data,
    const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
    const std::string& in_worksheet_name,
    const Cursor& in_cursor
    )
{
    const auto found = in_worksheets.find(in_worksheet_name);
    const auto worksheet = found != in_worksheets.end() ? found->second.get() : nullptr;
    if (nullptr == worksheet)
    {
        printf("failed to find worksheet:%s [Deal3rd]\n", in_worksheet_name.c_str());
        return false;
    }

    const int row_count = worksheet->GetRowCount();
    if (row_count <= 0)
    {
        printf("empty row worksheet:%s [Deal3rd]\n", in_worksheet_name.c_str());
        return false;
    }

    std::vector<std::string> key_row;
    for (int index = 0; index < worksheet->GetRowCellCount(0); ++index)
    {
        key_row.push_back(worksheet->GetCell(0, index));
    }

    if (static_cast<int>(key_row.size()) <= 0)
    {
        printf("empty key worksheet:%s [Deal3rd]\n", in_worksheet_name.c_str());
        return false;
    }

    const std::string first_key = key_row[0];
    std::vector<std::string> split_first_key;
    SplitString(split_first_key, first_key, ':');
    if (false == Contains(split_first_key, "_id"))
    {
        printf("first key should contain _id:%s [%s] [Deal3rd]\n", in_worksheet_name.c_str(), first_key.c_str());
        return false;
    }

    // deal with the other rows past the first
    for (int index = 1; index < row_count; ++index)
    {
        Sheet3rdNormalisedRow(
            out_data,
            in_worksheets,
            in_cursor,
            key_row, 
            *worksheet, 
            index
            );
    }

    return true;
}

//const bool WorksheetToJson::Deal3rdArray(
//    nlohmann::json& out_data,
//    const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
//    const std::string& in_worksheet_name,
//    const Cursor& in_cursor
//    )
//{
//    return false;
//}

const bool WorksheetToJson::Deal5th(
    nlohmann::json& out_data,
    const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
    const std::string& in_worksheet_name,
    const Cursor& in_cursor
    )
{
    const auto found = in_worksheets.find(in_worksheet_name);
    const auto worksheet = found != in_worksheets.end() ? found->second.get() : nullptr;
    if (nullptr == worksheet)
    {
        printf("failed to find worksheet:%s [Deal5th]\n", in_worksheet_name.c_str());
        return false;
    }

    const int row_count = worksheet->GetRowCount();
    if (row_count <= 0)
    {
        printf("empty row worksheet:%s [Deal5th]\n", in_worksheet_name.c_str());
        return false;
    }

    for (int index = 0; index < row_count; ++index)
    {
        Sheet5thNormalisedRow(
            out_data,
            in_worksheets,
            in_cursor,
            *worksheet, 
            index
            );
    }

    return true;
}

const bool WorksheetToJson::Deal3rdKeyValue(
    nlohmann::json& out_data,
    const std::map<std::string, std::shared_ptr<Worksheet>>& in_worksheets, 
    const std::string& in_worksheet_name,
    const Cursor& in_cursor
    )
{
    const auto found = in_worksheets.find(in_worksheet_name);
    const auto worksheet = found != in_worksheets.end() ? found->second.get() : nullptr;
    if (nullptr == worksheet)
    {
        printf("failed to find worksheet:%s [Deal3rd]\n", in_worksheet_name.c_str());
        return false;
    }

    const int row_count = worksheet->GetRowCount();
    if (row_count <= 0)
    {
        printf("empty row worksheet:%s [Deal3rd]\n", in_worksheet_name.c_str());
        return false;
    }

    std::vector<std::string> key_row;
    for (int index = 0; index < worksheet->GetRowCellCount(0); ++index)
    {
        key_row.push_back(worksheet->GetCell(0, index));
    }

    if (static_cast<int>(key_row.size()) <= 0)
    {
        printf("empty key worksheet:%s [Deal3rd]\n", in_worksheet_name.c_str());
        return false;
    }

    const std::string first_key = key_row[0];
    std::vector<std::string> split_first_key;
    SplitString(split_first_key, first_key, ':');
    if (false == Contains(split_first_key, "_id"))
    {
        printf("first key should contain _id:%s [%s] [Deal3rd]\n", in_worksheet_name.c_str(), first_key.c_str());
        return false;
    }

    // deal with the other rows past the first
    for (int index = 1; index < row_count; ++index)
    {
        Sheet3rdNormalisedRowKeyValue(
            out_data,
            in_worksheets,
            in_cursor,
            key_row, 
            *worksheet, 
            index
            );
    }

    return true;
}


