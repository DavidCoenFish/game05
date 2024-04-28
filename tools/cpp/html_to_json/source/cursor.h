#pragma once

#include <json\json.hpp>
#include <set>
#include <string>
#include <vector>

class Cursor
{
public:
    enum class Type
    {
        TNone,
        TMember,
        TArray
    };
    struct Data
    {
        Type type = Type::TNone;
        std::string member_key = {};
        int array_index = 0;
    };

    explicit Cursor(
        const std::set<std::string>& in_data_set = {},
        const std::vector<Data>& in_stack = {}
        );

    void Clear();
    void PushMember(const std::string& in_member_key);
    void PushArray(const int in_array_index);
    const bool TestDataset(const std::set<std::string>& in_data_set) const;

    Cursor Clone() const;

    //template<typename TYPE>
    //nlohmann::json SetValue(TYPE in_value, nlohmann::json& in_base_object)
    
    //nlohmann::json SetValueBool(const bool in_value, nlohmann::json& in_base_object);
    //nlohmann::json SetValueInt(const int in_value, nlohmann::json& in_base_object);
    //nlohmann::json SetValueFloat(const float in_value, nlohmann::json& in_base_object);
    //nlohmann::json SetValueString(const std::string& in_value, nlohmann::json& in_base_object);

    nlohmann::json SetValue(const nlohmann::json& in_value, nlohmann::json& in_base_object);

    //const int GetArrayLength(nlohmann::json& in_base_object);

private:
    std::set<std::string> _data_set;
    std::vector<Data> _stack;
};
