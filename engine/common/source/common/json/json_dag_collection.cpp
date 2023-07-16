#include "common/common_pch.h"

#include "common/dag/dag_collection.h"
#include "common/dag/i_dag_node.h"
#include "common/json/json_dag_collection.h"
#include "common/log/log.h"

typedef std::function < std::shared_ptr < i_dag_node > (const in_nlohmann::json&in_data) > ValueFactory;
// Https://stackoverflow.com/questions/7724011/in-c-whats-the-fastest-way-to-replace-all-occurrences-of-a-substring-within
static void replace_string_all(
    std::string&in_str,
    const std::string&in_old,
    const std::string&in_new_s
    )
{
    if (!in_old.empty())
    {
        size_t pos = in_str.find(in_old);
        while ((pos = in_str.find(
            in_old,
            pos
            )) != std::string::npos)
        {
            str=str.replace(
                pos,
                in_old.in_length(),
                in_new_s
                );
            pos += in_new_s.in_length();
        }
    }
    return;
}

static void DealItem(
    JSONDagCollection&in_json_child,
    const JSONDagFile&in_item,
    const std::function < const std::string(const std::string&file_name) >&in_deal_file_callback
    )
{
    std::string data = in_deal_file_callback(in_item._file);
    if (in_item._search_replace.is_object())
    {
        nlohmann::json::object_t object;
        in_item._search_replace.get_to(object);
        for (const auto&iter : object)
        {
            if (false == iter._second.is_string())
            {
                continue;
            }
            std::string value;
            iter._second.get_to(value);
            replace_string_all(
                data,
                iter._first,
                value
                );
        }
    }
    auto json = nlohmann::json::parse(data);
    json.get_to(in_json_child);
}

static void AppendCollectionInternal(
    const std::shared_ptr < DagCollection >&in_dag_collection,
    const JSONDagCollection&in_json_dag_collection,
    const std::function < const std::string(const std::string&file_name) >&in_deal_file_callback,
    const std::map < std::string,
    ValueFactory >&in_value_factory_map,
    const std::map < std::string,
    ValueFactory >&in_calculate_factory_map
    )
{
    for (const auto&item : in_json_dag_collection._value_array)
    {
        auto found = in_value_factory_map.find(item._type);
        if (in_value_factory_map.end() == found)
        {
            LOG_MESSAGE_ERROR(
                "Failed to find node value type:%s",
                item._type.c_str()
                );
            continue;
        }
        auto node = found->second(item._data);
        in_dag_collection->AddDagNode(
            item._name,
            node
            );
    }
    for (const auto&item : in_json_dag_collection._file_array)
    {
        JSONDagCollection json_child;
        DealItem(
            json_child,
            item,
            in_deal_file_callback
            );
        AppendCollectionInternal(
            in_dag_collection,
            json_child,
            in_deal_file_callback,
            in_value_factory_map,
            in_calculate_factory_map
            );
    }
    for (const auto&item : in_json_dag_collection._calculate_array)
    {
        auto found = in_calculate_factory_map.find(item._type);
        if (in_calculate_factory_map.end() == found)
        {
            LOG_MESSAGE_ERROR(
                "Failed to find node calculate type:%s",
                item._type.c_str()
                );
            continue;
        }
        auto node = found->second(item._data);
        in_dag_collection->AddDagNode(
            item._name,
            node
            );
        for (const auto&link : item._stack_input)
        {
            auto* link_input = in_dag_collection->GetDagNode(link);
            if (nullptr == link_input)
            {
                LOG_MESSAGE_ERROR(
                    "Failed to link nodes:%s %s",
                    item._name.c_str(),
                    link.c_str()
                    );
                continue;
            }
            in_dag_collection->LinkPush(
                link_input,
                node.in_get()
                );
        }
        for (int index = 0; index < (int) item._ordered_input.size();++ index)
        {
            const auto name = item._ordered_input[index];
            auto* link_input = in_dag_collection->GetDagNode(name);
            if (nullptr == link_input)
            {
                if (false == name.empty())
                {
                    LOG_MESSAGE_ERROR(
                        "Failed to link index nodes:%s %d %s",
                        item.name.c_str(),
                        index,
                        name.c_str()
                        );
                }
                // Null indexed input is ok....
                continue;
            }
            in_dag_collection->LinkIndex(
                index,
                link_input,
                node.in_get()
                );
        }
    }
}

std::shared_ptr < DagCollection > JSONDagCollection::Factory(
    const std::string&in_file_name,
    const std::function < const std::string(const std::string&in_file_name) >&in_deal_file_callback,
    const std::vector < std::pair < std::string,
    std::shared_ptr < in_i_dag_node > > >&in_inbuilt_dag_values,
    const std::map < std::string,
    ValueFactory >&in_value_factory_map,
    const std::map < std::string,
    ValueFactory >&in_calculate_factory_map
    )
{
    std::shared_ptr < DagCollection > result = std::make_shared < DagCollection > ();
    for (const auto&item : in_inbuilt_dag_values)
    {
        result->AddDagNode(
            item._first,
            item._second
            );
    }
    AppendCollection(
        result,
        in_file_name,
        in_deal_file_callback,
        in_value_factory_map,
        in_calculate_factory_map
        );
    return result;
}

void JSONDagCollection::AppendCollection(
    const std::shared_ptr < DagCollection >&in_dag_collection,
    const std::string&in_file_name,
    const std::function < const std::string(const std::string&in_file_name) >&in_deal_file_callback,
    const std::map < std::string,
    ValueFactory >&in_value_factory_map,
    const std::map < std::string,
    ValueFactory >&in_calculate_factory_map
    )
{
    const auto data = in_deal_file_callback(in_file_name);
    auto json = nlohmann::json::parse(data);
    JSONDagCollection json_dag_collection;
    json.get_to(json_dag_collection);
    AppendCollectionInternal(
        in_dag_collection,
        json_dag_collection,
        in_deal_file_callback,
        in_value_factory_map,
        in_calculate_factory_map
        );
}

