#include "common/common_pch.h"
#include "common/dag/threaded/dag_threaded.h"

#include "common/dag/threaded/i_dag_threaded_node.h"
#include "common/dag/threaded/i_dag_threaded_visitor.h"
#include "common/dag/threaded/dag_threaded_value.h"
#include "common/dag/threaded/dag_threaded_node_calculate.h"
#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/locale/locale_enum.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_string_format_map.h"
#include "common/tooltip/tooltip.h"
#include "common/tooltip/tooltip_data.h"
 
namespace
{
	constexpr char s_locale_key_tooltip_text[] = "c_dag_tooltip_text";
	//"{name}({value})";

	class NodeVisitor : public IDagThreadedVisitor
	{
	public:
		NodeVisitor(const int32_t in_index, const LocaleSystem& in_locale_system, const DagThreadedCollection& in_collection) 
		: _index(in_index)
		, _locale_system(in_locale_system)
		, _collection(in_collection)
		{}
		const std::string& GetDisplayName() const { return _display_name; }
		const std::shared_ptr<Tooltip>& GetTooltip() const { return _tooltip; }

	private:
	virtual const bool OnCalculate(
		const std::shared_ptr<IDagThreadedValue>&,
		const std::string&,
		const std::string&,
		const std::string&,
		const std::vector<NodeID>&,
		const std::vector<NodeID>& in_array_input_index,
		const std::vector<NodeID>&
		) override
	{
		if ((0 <= _index) && (_index <static_cast<int32_t>(in_array_input_index.size())))
		{
			IDagThreadedNode* node = in_array_input_index[_index];
			if (nullptr != node)
			{
				_display_name = DagThreaded::GetTooltipName(node, _locale_system);
				_tooltip = DagThreaded::GetTooltipBody(_collection, node, _locale_system);
			}
		}
		return false;
	}

	virtual const bool OnVariable(
		const std::shared_ptr<IDagThreadedValue>&,
		const std::string&,
		const std::string&
		) override
		{
			DSC_ASSERT_ALWAYS("invalid code path");
			return false;
		}
	private:
		const DagThreadedCollection& _collection;
		const LocaleSystem& _locale_system;
		const int32_t _index = 0;
		std::string _display_name;
		std::shared_ptr<Tooltip> _tooltip;
	};

	class TooltipBuilder : public ILocaleStringFormat
	{
	public:
		TooltipBuilder(
			const DagThreadedCollection& in_collection,
			const NodeID in_node_id
			)
		: _current_level(0)
		, _collection(in_collection)
		, _node_id(in_node_id)
		{
			// nop
		}
		const std::shared_ptr<Tooltip> GetResult()
		{ 
			if (nullptr == _tooltip)
			{
				_tooltip = std::make_shared<Tooltip>(_data);
			}
			return _tooltip; 
		}
	private:
		void AccumulateString(const std::string& in_value) override
		{
			_data.push_back(TooltipData({in_value, {}, _current_level}));
		}

		void AccumulateToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) override
		{
			in_locale;
			in_index;
			//does token start with "_self"
			if (in_token == "_self")
			{
				if (nullptr != _node_id)
				{
					const std::string value = DagThreaded::GetTooltipText(_collection, _node_id, in_locale_system);
					// self should not link (else recusion)
					_data.push_back(TooltipData({value, {}, _current_level}));
				}
				return;
			}
			//does token start with "_index."
			if (in_token.rfind("_index.", 0))
			{
				const std::string sub_string = in_token.substr(7);
				const int32_t index = std::stoi(sub_string);
				DagThreadedNodeCalculate* node = dynamic_cast<DagThreadedNodeCalculate*>(_node_id);
				if (node)
				{
					NodeVisitor visitor(index, in_locale_system, _collection);
					_node_id->Visit(visitor);
					_data.push_back(TooltipData({visitor.GetDisplayName(), visitor.GetTooltip(), _current_level}));
				}
				return;
			}

			//does token start with "_level."
			if (in_token.rfind("_level.", 0))
			{
				const std::string sub_string = in_token.substr(7);
				_current_level = std::stoi(sub_string);
				return;
			}
			//search for node in collection by name token
			{
				NodeID new_node = _collection.FindNode(in_token);
				if (nullptr != new_node)
				{
					const std::string value = DagThreaded::GetTooltipText(_collection, new_node, in_locale_system);
					const std::shared_ptr<Tooltip> tooltip = DagThreaded::GetTooltipBody(_collection, new_node, in_locale_system);
					_data.push_back(TooltipData({value, tooltip, _current_level}));
				}
				return;
			}
		}

	private:
		const DagThreadedCollection& _collection;
		NodeID _node_id;
		std::vector<TooltipData> _data;
		std::shared_ptr<Tooltip> _tooltip;
		int _current_level;
	};
}

template <>
const bool DagThreaded::IsZero<std::string>(const std::string& value)
{
	return value.empty();
}

void DagThreaded::RegisterLocaleSystem(LocaleSystem& in_out_locale_system)
{
	const std::vector<LocaleSystem::Data> data = {
		{s_locale_key_tooltip_text, "{name}({value})"}
		};
	in_out_locale_system.Append(LocaleISO_639_1::Default, data);

}

/// example "Damage Tollerance"
const std::string DagThreaded::GetTooltipName(const NodeID in_node_id, const LocaleSystem& in_locale_system)
{
	const std::string key = in_node_id ? in_node_id->GetDisplayName() : std::string();
	const std::string result = in_locale_system.GetValue(key);
	return result;
}

/// example "Damage Tollerance(17)"
const std::string DagThreaded::GetTooltipText(const DagThreadedCollection& in_collection, const NodeID in_node_id, const LocaleSystem& in_locale_system)
{
	in_collection;
	std::string value;

	const std::shared_ptr<IDagThreadedValue> dag_value = in_node_id ? in_node_id->GetValue() : nullptr;
	auto dag_value_int = dag_value ? dynamic_cast<DagThreadedValue<int32_t>*>(dag_value.get()) : nullptr;
	if (nullptr != dag_value_int)
	{
		value = std::to_string(dag_value_int->Get());
	}
	if (value.empty())
	{
		auto dag_value_string = dag_value ? dynamic_cast<DagThreadedValue<std::string>*>(dag_value.get()) : nullptr;
		if (nullptr != dag_value_string)
		{
			value = dag_value_string->Get();
		}
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
	in_locale_system.GetValueFormatted(s_locale_key_tooltip_text, string_formatting);

	const std::string result = string_formatting.GetResult();
	return result;
}

/// example "Damage Tollerance(17) = 12 + 1d12"
const std::shared_ptr<Tooltip> DagThreaded::GetTooltipBody(const DagThreadedCollection& in_collection, const NodeID in_node_id, const LocaleSystem& in_locale_system)
{
	TooltipBuilder tooltip_builder(in_collection, in_node_id);
	in_locale_system.GetValueFormatted(in_node_id->GetTooltipRaw(), tooltip_builder);
	return tooltip_builder.GetResult();

}
