#pragma once

#include <json\json.hpp>
#include <set>
#include <string>
#include <vector>

/// Cursor is a construction aid form making a json document, it tracks a location in a json hierarchy and allows you to
/// set a value at a given location, under n x [dictionary member, array index]
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
        const bool in_use_data_set = false,
        const std::vector<Data>& in_stack = {}
        );

    void Clear();
    void PushMember(const std::string& in_member_key);
    void PushArray(const int in_array_index);
    //const bool TestDataset(const std::set<std::string>& in_data_set) const;
    const bool TestDataset(const std::string& in_needle) const;

    Cursor Clone() const;
    Cursor CloneSetUseDataSet(const bool in_use_data_set) const;

    /// invalue should just be a json plan old data, objects and arrays should be appended to cursor stack not a value
    void SetValue(nlohmann::json& in_out_base_object, const nlohmann::json& in_value) const;

    const int GetArrayLength(nlohmann::json& in_out_base_object) const;

private:
    std::set<std::string> _data_set;
    bool _use_data_set;
    std::vector<Data> _stack;
};
