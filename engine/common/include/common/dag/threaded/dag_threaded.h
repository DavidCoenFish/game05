#pragma once

class DagThreadedCollection;
class IDagThreadedNode;
class IDagThreadedValue;
class LocaleSystem;
class ITooltip;
typedef IDagThreadedNode* NodeID;
enum class LocaleISO_639_1;

namespace DagThreaded
{
	enum DirtyCase
	{
		None = 0,
		ValueChanged = 1 << 0,
		NewValueNotZero = 1 << 1
	};

	template <typename TYPE>
	const bool IsZero(const TYPE& value)
	{
		return (0 == value);
	}

	template <>
	const bool IsZero<std::string>(const std::string& value);


	static void RegisterLocaleSystem(LocaleSystem& in_locale_system);

	/// example "Damage Tollerance"
	//const std::string GetTooltipName(const NodeID in_node_id, const LocaleSystem& in_locale_system);
	/// example "Damage Tollerance(17)"
	//const std::string GetTooltipText(const DagThreadedCollection& in_collection, const NodeID in_node_id, const LocaleSystem& in_locale_system);
	/// example "Damage Tollerance(17) = 12 + 1d12"
	//const std::shared_ptr<Tooltip> GetTooltipBody(const DagThreadedCollection& in_collection, const NodeID in_node_id, const LocaleSystem& in_locale_system);

	//const std::shared_ptr<ITooltip> GetTooltip(const DagThreadedCollection& in_collection, const NodeID in_node_id, const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale);

	// return null if there is no data for tooltip, ie, in_tooltip_locale_key_text_or_empty or what it returns from locale system is empty
	const std::shared_ptr<ITooltip> BuildTooltip(
		const DagThreadedCollection& in_collection, 
		const LocaleSystem& in_locale_system, 
		const LocaleISO_639_1 in_locale,
		const std::string& in_tooltip_locale_key_text_or_empty,
		const std::string& in_tooltip_locale_key_children_or_empty,
		const std::shared_ptr<IDagThreadedValue>& in_value,
		const std::vector<IDagThreadedNode*> in_array_input_stack = {},
		const std::vector<IDagThreadedNode*> in_array_input_index = {}
		);

};
