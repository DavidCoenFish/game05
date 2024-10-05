#include "common/common_pch.h"
#include "common/dag/threaded/dag_threaded.h"

#include "common/dag/threaded/i_dag_threaded_node.h"
#include "common/dag/threaded/dag_threaded_value.h"
#include "common/locale/locale_enum.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_string_format_map.h"
 
namespace
{
    constexpr char s_locale_key_tooltip_text[] = "c_dag_tooltip_text";
    //"{name}({value})";
}


void DagThreaded::RegisterLocaleSystem(LocaleSystem& in_out_locale_system)
{
    const std::vector<LocaleSystem::Data> data = {
        {s_locale_key_tooltip_text, "{name}({value})"}
        };
    in_out_locale_system.Append(LocaleISO_639_1::Default, data);

}

/// example "Damage Tollerance"
const std::string DagThreaded::GetTooltipName(const NodeID in_node_id, LocaleSystem& in_locale_system)
{
    const std::string key = in_node_id ? in_node_id->GetDisplayName() : std::string();
    const std::string result = in_locale_system.GetValue(key);
    return result;
}

/// example "Damage Tollerance(17)"
const std::string DagThreaded::GetTooltipText(const DagThreadedCollection& in_collection, const NodeID in_node_id, LocaleSystem& in_locale_system)
{
    std::string value;

    const std::shared_ptr<IDagThreadedValue> dag_value = in_node_id ? in_node_id->GetValue() : nullptr;
    auto dag_value_int = dag_value ? dynamic_cast<DagThreadedValue<int32_t>*>(dag_value.get()) : nullptr;
    if (nullptr != dag_value_int)
    {
        value = std::to_string(dag_value_int->Get());
    }
    auto dag_value_string = dag_value ? dynamic_cast<DagThreadedValue<std::string>*>(dag_value.get()) : nullptr;
    if (nullptr != dag_value_string)
    {
        value = dag_value_string->Get();
    }

    const std::string name = GetTooltipName(in_node_id, in_locale_system);
    if (value.empty())
    {
        return name;
    }

    std::map<std::string, std::string> format_map;
    format_map["value"] = value;
    format_map["name"] = name;
    LocaleStringFormatMap string_formatting(format_map);
    in_locale_system.GetValueFormatted("c_dag_tooltip_text", string_formatting);

    const std::string result = string_formatting.GetResult();
    return result;
}

/// example "Damage Tollerance(17) = 12 + 1d12"
const std::string DagThreaded::GetTooltipBody(const DagThreadedCollection& in_collection, const NodeID in_node_id, LocaleSystem& in_locale_system)
{
}
