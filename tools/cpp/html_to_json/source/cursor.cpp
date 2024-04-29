#include "cursor.h"

namespace
{
    nlohmann::json MakePath(const std::vector<Cursor::Data>& in_stack, const int in_index, const nlohmann::json& in_value, nlohmann::json& in_parent)
    {
        if (0 < in_stack.size())
        {
            const auto item = in_stack[in_index];

            switch(item.type)
            {
            default:
                printf("Error Cursor::MakePath\n");
                break;
            case Cursor::Type::TMember:
                {
                    nlohmann::json trace;
                    if (false == in_parent.is_object())
                    {
                        in_parent = nlohmann::json::object();
                    }

                    auto found = in_parent.find(item.member_key);
                    if (found != in_parent.end())
                    {
                        trace = *found;
                    }
                    else
                    {
                        trace = {};
                    }

                    in_parent[item.member_key] = MakePath(in_stack, in_index + 1, in_value, trace);
                }
                break;
            case Cursor::Type::TArray:
                {
                    nlohmann::json trace;
                    if (false == in_parent.is_array())
                    {
                        in_parent = nlohmann::json::array();
                    }

                    const int max_size = std::max<int>(static_cast<int>(in_parent.size()), item.array_index + 1);
                    for (int index = static_cast<int>(in_parent.size()); index < max_size; ++index)
                    {
                        in_parent.push_back(0);
                    }
                    trace = in_parent[item.array_index];

                    in_parent[item.array_index] = MakePath(in_stack, in_index + 1, in_value, trace);
                }
                break;
            }
        }
        else
        {
            return in_value;
        }
        return in_parent;
    }
}

Cursor::Cursor(
    const std::set<std::string>& in_data_set,
    const std::vector<Data>& in_stack
    )
    : _data_set(in_data_set)
    , _stack(in_stack)
{
    // nop
}

void Cursor::Clear()
{
    _stack.clear();
    return;
}

void Cursor::PushMember(const std::string& in_member_key)
{
    _stack.push_back(Data({Type::TMember, in_member_key, 0}));
    return;
}

void Cursor::PushArray(const int in_array_index)
{
    _stack.push_back(Data({Type::TArray, {}, in_array_index}));
    return;
}

const bool Cursor::TestDataset(const std::set<std::string>& in_data_set) const
{
    for (auto item : _data_set)
    {
        auto found = in_data_set.find(item);
        if (found != in_data_set.end())
        {
            return true;
        }
    }

    return false;
}

Cursor Cursor::Clone() const
{
    return Cursor(_data_set, _stack);
}

//template<typename TYPE>
//nlohmann::json SetValue(TYPE in_value, nlohmann::json& in_base_object)
    
//nlohmann::json SetValueBool(const bool in_value, nlohmann::json& in_base_object);
//nlohmann::json SetValueInt(const int in_value, nlohmann::json& in_base_object);
//nlohmann::json SetValueFloat(const float in_value, nlohmann::json& in_base_object);
//nlohmann::json SetValueString(const std::string& in_value, nlohmann::json& in_base_object);

void Cursor::SetValue(nlohmann::json& in_out_base_object, const nlohmann::json& in_value)
{
    if (0 < _stack.size())
    {
        const auto item = _stack[0];

        switch(item.type)
        {
        default:
            printf("Error Cursor::SetValue\n");
            break;
        case Type::TMember:
            {
                nlohmann::json trace;
                if (false == in_out_base_object.is_object())
                {
                    in_out_base_object = nlohmann::json::object();
                }

                auto found = in_out_base_object.find(item.member_key);
                if (found != in_out_base_object.end())
                {
                    trace = *found;
                }
                in_out_base_object[item.member_key] = MakePath(_stack, 1, in_value, trace);
            }
            break;
        case Type::TArray:
            {
                nlohmann::json trace;
                if (false == in_out_base_object.is_array())
                {
                    in_out_base_object = nlohmann::json::array();
                }
                const int max_size = std::max<int>(static_cast<int>(in_out_base_object.size()), item.array_index + 1);
                for (int index = static_cast<int>(in_out_base_object.size()); index < max_size; ++index)
                {
                    in_out_base_object.push_back(0);
                }
                trace = in_out_base_object[item.array_index];

                in_out_base_object[item.array_index] = MakePath(_stack, 1, in_value, trace);
            }
            break;
        }

    }

    return;
}
