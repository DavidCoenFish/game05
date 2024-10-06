#pragma once

class DagThreadedCollection;
class IDagThreadedNode;
class LocaleSystem;
class Tooltip;
typedef IDagThreadedNode* NodeID;

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
	const std::string GetTooltipName(const NodeID in_node_id, const LocaleSystem& in_locale_system);
	/// example "Damage Tollerance(17)"
	const std::string GetTooltipText(const DagThreadedCollection& in_collection, const NodeID in_node_id, const LocaleSystem& in_locale_system);
	/// example "Damage Tollerance(17) = 12 + 1d12"
	const std::shared_ptr<Tooltip> GetTooltipBody(const DagThreadedCollection& in_collection, const NodeID in_node_id, const LocaleSystem& in_locale_system);

};
