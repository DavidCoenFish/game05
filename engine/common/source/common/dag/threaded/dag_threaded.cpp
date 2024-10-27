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
#include "common/tooltip/i_tooltip.h"
#include "common/tooltip/tooltip_link.h"
#include "common/tooltip/tooltip_text.h"
 
namespace
{
	constexpr char s_locale_key_true[] = "c_dag_true";
	constexpr char s_locale_key_false[] = "c_dag_false";

	class TooltipTextBuilder : public ILocaleStringFormat
	{
	public:
		TooltipTextBuilder(
			std::string& out_text,
			const std::shared_ptr<IDagThreadedValue>& in_value
			)
		: _text(out_text)
		, _value(in_value)
		{
			// nop
		}
	private:
		void AccumulateString(const std::string& in_value, const LocaleISO_639_1) override
		{
			_text += in_value;
		}

		void AccumulateToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int) override
		{
			auto dag_value_int = dynamic_cast< DagThreadedValue< int32_t >* >(_value.get());
			auto dag_value_string = dynamic_cast< DagThreadedValue< std::string >* >(_value.get());

			// having some conceptual issues with having verbosity set in the text of the tooltip, more of a child thing
			if (in_token == "_value")
			{
				if (nullptr != dag_value_int)
				{
					_text += std::to_string(dag_value_int->Get());
				}
				else if (nullptr != dag_value_string)
				{
					_text += dag_value_string->GetRef();
				}
			}
			else if (in_token == "_value_boolean")
			{
				if (nullptr != dag_value_int)
				{
					const bool value = 0 != dag_value_int->Get();
					if (true == value)
					{
						_text += in_locale_system.GetValue(in_locale, s_locale_key_true);
					}
					else
					{
						_text += in_locale_system.GetValue(in_locale, s_locale_key_false);
					}

					_text += std::to_string(dag_value_int->Get());
				}

			}
		}

	private:
		std::string& _text;
		const std::shared_ptr<IDagThreadedValue>& _value;

	};

	class TooltipChildrenBuilder : public ILocaleStringFormat
	{
	public:
		TooltipChildrenBuilder(
			std::vector<std::shared_ptr<ITooltip>>& out_children,
			const std::string& in_text,
			const DagThreadedCollection& in_collection, 
			const std::vector<IDagThreadedNode*> in_array_input_stack,
			const std::vector<IDagThreadedNode*> in_array_input_index
			)
			: _children(out_children)
			, _text(in_text)
			, _verbosity(0)
			, _collection(in_collection)
			, _array_input_stack(in_array_input_stack)
			, _array_input_index(in_array_input_index)
		{
			// nop
		}
	private:
		void AccumulateString(const std::string& in_value, const LocaleISO_639_1 in_locale) override
		{
			_children.push_back(std::make_shared<TooltipText>(in_value, in_locale, _verbosity));
		}
		void AccumulateToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int) override
		{
			if (std::string::npos != in_token.rfind("_v.", 0))
			{
				const std::string sub_string = in_token.substr(3);
				_verbosity = std::stoi(sub_string);
				return;
			}
			else if (in_token == "_self")
			{
				_children.push_back(std::make_shared<TooltipText>(_text, in_locale, _verbosity));
				return;
			}
			//does token start with "_index."
			else if (std::string::npos != in_token.rfind("_index.", 0))
			{
				const std::string sub_string = in_token.substr(7);
				const int32_t index = std::stoi(sub_string);
				if ((0 <= index) && (index < _array_input_index.size()) && (nullptr != _array_input_index[index]))
				{
					_children.push_back(_array_input_index[index]->GetTooltip(_collection, in_locale_system, in_locale));
				}
			}
			else
			{
				// on reusing tooltip of other nodes, we don't use our current verbosity level, unless we make something to clone the tooltip at a different verbosity?
				auto tooltip = _collection.GetTooltip(_collection.FindNode(in_token), in_locale_system, in_locale);
				if (nullptr != tooltip)
				{
					_children.push_back(tooltip);
				}
			}
		}

	private:
		std::vector<std::shared_ptr<ITooltip>>& _children;
		const std::string _text = {};
		int32_t _verbosity = 0;
		const DagThreadedCollection& _collection;
		const std::vector<IDagThreadedNode*>& _array_input_stack;
		const std::vector<IDagThreadedNode*>& _array_input_index;

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
		{s_locale_key_true, "True"},
		{s_locale_key_false, "False"},
		};
	in_out_locale_system.Append(LocaleISO_639_1::Default, data);
}

/*
in_tooltip_locale_key_text_or_empty
"Damage Tollerance(17)" via "{_key_damage_tollerance}({_value})" => "Damage Tollerance({_value})"
"Alive(True)" via "{_key_alive}({_value_boolean})" => "Alive({_value_boolean})"
move away from the seconday lookup of locale key inside a locale key?

in_tooltip_locale_key_children_or_empty
"Damage Tollerance(17) = 12 + 1d12" via "{_self} = {_index.1} + {_index.2}d{_index.3}"

*/

const std::shared_ptr<ITooltip> DagThreaded::BuildTooltip(
	const DagThreadedCollection& in_collection, 
	const LocaleSystem& in_locale_system, 
	const LocaleISO_639_1 in_locale,
	const std::string& in_tooltip_locale_key_text_or_empty,
	const std::string& in_tooltip_locale_key_children_or_empty,
	const std::shared_ptr<IDagThreadedValue>& in_value,
	const std::vector<IDagThreadedNode*> in_array_input_stack,
	const std::vector<IDagThreadedNode*> in_array_input_index
	)
{
	// build text
	if (true == in_tooltip_locale_key_text_or_empty.empty())
	{
		return nullptr;
	}

	std::string text = {};
	int32_t verbosity = 0;
	TooltipTextBuilder tooltip_text_builder(text, in_value);
	in_locale_system.GetValueFormatted(in_tooltip_locale_key_text_or_empty, tooltip_text_builder);
	if (true == text.empty())
	{
		return nullptr;
	}

	if (false == in_tooltip_locale_key_children_or_empty.empty())
	{
		std::vector<std::shared_ptr<ITooltip>> children = {};
		TooltipChildrenBuilder tooltip_children_builder(children, text, in_collection, in_array_input_stack, in_array_input_index);
		in_locale_system.GetValueFormatted(in_tooltip_locale_key_children_or_empty, tooltip_children_builder);
		if (0 < children.size())
		{
			return std::make_shared<TooltipLink>(text, in_locale, verbosity, children);
		}
	}

	return std::make_shared<TooltipText>(text, in_locale, verbosity);
}

