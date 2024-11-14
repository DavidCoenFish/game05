#include "common/common_pch.h"
#include "common/dag/threaded/dag_threaded_node_calculate.h"

#include "common/dag/threaded/dag_threaded.h"
#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_value.h"
#include "common/dag/threaded/i_dag_threaded_visitor.h"
#include "common/locale/locale_system.h"
#include "common/tooltip/i_tooltip.h"
#include "common/tooltip/tooltip_builder.h"
#include "common/tooltip/tooltip_text.h"

namespace
{
	class DagTooltipDataSource : public ITooltipBuilderDataSource
	{
	public:
		DagTooltipDataSource(
			const DagThreadedCollection& in_collection,
			const std::shared_ptr<IDagThreadedValue>& in_value,
			const std::vector<IDagThreadedNode*>& in_array_input_stack,
			const std::vector<IDagThreadedNode*>& in_array_input_index
			)
			: _collection(in_collection)
			, _value(in_value)
			, _array_input_stack(in_array_input_stack)
			, _array_input_index(in_array_input_index)
		{
			// Nop
		}
	virtual const TValue GetValue() override
	{
		auto dag_value_int = dynamic_cast< DagThreadedValue< int >* >(_value.get());
		if (nullptr != dag_value_int)
		{
			return TValue((int)dag_value_int->Get());
		}

		auto dag_value_bool = dynamic_cast< DagThreadedValue< bool >* >(_value.get());
		if (nullptr != dag_value_bool)
		{
			return TValue((bool)dag_value_bool->Get());
		}

		auto dag_value_string = dynamic_cast< DagThreadedValue< std::string >* >(_value.get());
		if (nullptr != dag_value_string)
		{
			TValue(dag_value_string->Get());
		}

		return TValue();
	}

	virtual const std::string GetText(
		const std::string& in_token, 
		const LocaleSystem& in_locale_system, 
		const LocaleISO_639_1 in_locale) override
	{
		std::string result = {};
		if (in_token == "_s_all_comma")
		{
			bool added = false;
			for (auto item : _array_input_stack)
			{
				if (nullptr == item)
				{
					continue;
				}
				std::shared_ptr<ITooltip> tooltip = item->GetTooltip(_collection, in_locale_system, in_locale);
				if (nullptr != tooltip)
				{
					if (added == true)
					{
						result += in_locale_system.GetValue(in_locale, TooltipBuilder::GetLocaleKeyCommaSeperator());
					}
					added = true;
					result += tooltip->GetText();
				}
			}
		}
		else if (std::string::npos != in_token.rfind("_s.", 0))
		{
			const std::string sub_string = in_token.substr(3);
			const int32_t index = std::stoi(sub_string);
			if ((0 <= index) && (index < _array_input_stack.size()) && (nullptr != _array_input_stack[index]))
			{
				std::shared_ptr<ITooltip> tooltip = _array_input_stack[index]->GetTooltip(_collection, in_locale_system, in_locale);
				if (nullptr != tooltip)
				{
					result = tooltip->GetText();
				}
			}
		}
		else if (std::string::npos != in_token.rfind("_i.", 0))
		{
			const std::string sub_string = in_token.substr(3);
			const int32_t index = std::stoi(sub_string);
			if ((0 <= index) && (index < _array_input_index.size()) && (nullptr != _array_input_index[index]))
			{
				std::shared_ptr<ITooltip> tooltip = _array_input_index[index]->GetTooltip(_collection, in_locale_system, in_locale);
				if (nullptr != tooltip)
				{
					result = tooltip->GetText();
				}
			}
		}
		else
		{
			DagThreadedCollection::NodeID node_id =_collection.FindNode(in_token);
			std::shared_ptr<ITooltip> tooltip = _collection.GetTooltip(node_id, in_locale_system, in_locale);
			if (nullptr != tooltip)
			{
				result = tooltip->GetText();
			}
		}
		return result;
	}

	virtual void AppendChildren(
		std::vector<std::shared_ptr<ITooltip>>& in_out_children,
		const int in_clone_verbosity,
		const std::string& in_token, 
		const LocaleSystem& in_locale_system, 
		const LocaleISO_639_1 in_locale) override
	{
		//_s_all_comma	list every tooltip of items in the stack input, comma seperated
		if (in_token == "_s_all_comma")
		{
			bool added = false;
			for (auto item : _array_input_stack)
			{
				if (nullptr == item)
				{
					continue;
				}
				std::shared_ptr<ITooltip> tooltip = item->GetTooltip(_collection, in_locale_system, in_locale);
				if (nullptr != tooltip)
				{
					if (added == true)
					{
						in_out_children.push_back(
						std::make_shared<TooltipText>(
							in_locale_system.GetValue(in_locale, TooltipBuilder::GetLocaleKeyCommaSeperator()),
							in_locale, in_clone_verbosity
							));
					}
					added = true;
					in_out_children.push_back(tooltip->Clone(in_clone_verbosity));
				}
			}
		}
		else if (std::string::npos != in_token.rfind("_s.", 0))
		{
			const std::string sub_string = in_token.substr(3);
			const int32_t index = std::stoi(sub_string);
			if ((0 <= index) && (index < _array_input_stack.size()) && (nullptr != _array_input_stack[index]))
			{
				std::shared_ptr<ITooltip> tooltip = _array_input_stack[index]->GetTooltip(_collection, in_locale_system, in_locale);
				if (nullptr != tooltip)
				{
					in_out_children.push_back(tooltip->Clone(in_clone_verbosity));
				}
			}
		}
		else if (std::string::npos != in_token.rfind("_i.", 0))
		{
			const std::string sub_string = in_token.substr(3);
			const int32_t index = std::stoi(sub_string);
			if ((0 <= index) && (index < _array_input_index.size()) && (nullptr != _array_input_index[index]))
			{
				std::shared_ptr<ITooltip> tooltip = _array_input_index[index]->GetTooltip(_collection, in_locale_system, in_locale);
				if (nullptr != tooltip)
				{
					in_out_children.push_back(tooltip->Clone(in_clone_verbosity));
				}
			}
		}
		else
		{
			DagThreadedCollection::NodeID node_id =_collection.FindNode(in_token);
			std::shared_ptr<ITooltip> tooltip = _collection.GetTooltip(node_id, in_locale_system, in_locale);
			if (nullptr != tooltip)
			{
				in_out_children.push_back(tooltip->Clone(in_clone_verbosity));
			}
		}
	}

	private:
		const DagThreadedCollection& _collection;
		std::shared_ptr<IDagThreadedValue> _value;
		std::vector<IDagThreadedNode*> _array_input_stack;
		std::vector<IDagThreadedNode*> _array_input_index;

	};
}


DagThreadedNodeCalculate::DagThreadedNodeCalculate(
	const std::string& in_uid,
	const CalculateFunction& in_calculate_function,
	const std::string& in_tooltip_locale_key_text,
	const std::string& in_tooltip_locale_key_children
	)
	: _uid(in_uid)
	, _calculate_function(in_calculate_function)
	, _tooltip_locale_key_text(in_tooltip_locale_key_text)
	, _tooltip_locale_key_children(in_tooltip_locale_key_children)
	, _tooltip_is_null(false)
{
	//nop
}

DagThreadedNodeCalculate::~DagThreadedNodeCalculate()
{
	//nop
}

void DagThreadedNodeCalculate::AddInputStack(IDagThreadedNode* const in_node)
{
	if (nullptr != in_node)
	{
		{
			std::shared_lock write_lock(_array_input_stack_mutex);
			_array_input_stack.push_back(in_node);
		}
		in_node->SetOutput(this);
	}
	MarkDirty();

	return;
}

void DagThreadedNodeCalculate::RemoveInputStack(IDagThreadedNode* const in_node)
{
	if (nullptr == in_node)
	{
		return;
	}
	in_node->RemoveOutput(this);

	{
		std::shared_lock write_lock(_array_input_stack_mutex);
		_array_input_stack.erase(std::remove(_array_input_stack.begin(), _array_input_stack.end(), in_node), _array_input_stack.end());
	}

	MarkDirty();

	return;
}

void DagThreadedNodeCalculate::SetInputIndex(IDagThreadedNode* const in_node, const int in_index)
{
	DSC_ASSERT(0 <= in_index, "invalid param");

	{
		std::shared_lock write_lock(_array_input_index_mutex);
		if (_array_input_index.size() <= in_index)
		{
			_array_input_index.resize((size_t)in_index + 1);
		}
		{
			auto const before = _array_input_index[in_index];
			if (nullptr != before)
			{
				before->RemoveOutput(this);
			}
		}
		_array_input_index[in_index] = in_node;
	}

	if (nullptr != in_node)
	{
		in_node->SetOutput(this);
	}

	MarkDirty();

	return;
}

void DagThreadedNodeCalculate::Unlink()
{
	// We don't make a paired lock of [_array_input_stack_mutex, _array_output_mutex] as it is a many to one linkage
	{
		std::shared_lock write_lock(_array_input_stack_mutex);
		for (auto pIter : _array_input_stack)
		{
			pIter->RemoveOutput(this);
		}
		_array_input_stack.clear();
	}

	{
		std::shared_lock write_lock(_array_input_index_mutex);
		for (auto pIter : _array_input_index)
		{
			pIter->RemoveOutput(this);
		}
		_array_input_index.clear();
	}

	return;
}

const std::string& DagThreadedNodeCalculate::GetUid() const
{
	return _uid;
}

void DagThreadedNodeCalculate::SetOutput(IDagThreadedNode* const in_node)
{
	if (nullptr != in_node)
	{
		std::shared_lock write_lock(_array_output_mutex);
		_array_output.push_back(in_node);
	}

	return;
}

void DagThreadedNodeCalculate::RemoveOutput(IDagThreadedNode* const in_node)
{
	std::shared_lock write_lock(_array_output_mutex);
	_array_output.erase(std::remove(_array_output.begin(), _array_output.end(), in_node), _array_output.end());

	return;
}

void DagThreadedNodeCalculate::MarkDirty()
{
	const int before_add_change_id = _change_id.fetch_add(1);
	const int calculate_id = _calculate_id.load();

	// only need to tell outputs we became dirty again when we change to be dirty
	if (before_add_change_id == calculate_id)
	{
		std::unique_lock read_lock(_array_output_mutex);
		for (auto pIter : _array_output)
		{
			pIter->MarkDirty();
		}
	}
	return;
}

std::shared_ptr<IDagThreadedValue> DagThreadedNodeCalculate::GetValue()
{
	const int change_id = _change_id.load();
	const int calculate_id = _calculate_id.exchange(change_id);
	if ((change_id != calculate_id) && (nullptr != _calculate_function))
	{
		std::vector< std::shared_ptr< IDagThreadedValue > > array_stack;
		{
			std::unique_lock read_lock(_array_input_stack_mutex);
			for(auto iter: _array_input_stack)
			{
				auto value = iter ? iter->GetValue() : nullptr;
				if (nullptr != value)
				{
					array_stack.push_back(value);
				}
			}
		}

		std::vector< std::shared_ptr< IDagThreadedValue > > array_indexed;
		{
			{
				std::unique_lock read_lock(_array_input_index_mutex);
				for(auto iter: _array_input_index)
				{
					auto value = iter ? iter->GetValue() : nullptr;
					array_indexed.push_back(value);
				}
			}
		}

		_value = _calculate_function(array_stack, array_indexed);
	}

	return _value;
}

std::shared_ptr<ITooltip> DagThreadedNodeCalculate::GetTooltip(const DagThreadedCollection& in_collection, const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale)
{
	const int change_id = _change_id.load();
	const int tooltip_id = _tooltip_id.exchange(change_id);
	std::shared_ptr<IDagThreadedValue> result;
	bool recalculate = (change_id != tooltip_id);
	// tooltip never generated/ tooltip doesn't generate as null
	if ((false == recalculate) && (false == _tooltip_is_null) && (nullptr == _tooltip))
	{
		recalculate = true;
	}
	// do we have a locale change
	if ((false == recalculate) && (nullptr != _tooltip) && (in_locale != _tooltip->GetLocale()))
	{
		recalculate = true;
	}

	if (true == recalculate)
	{
		std::shared_ptr<IDagThreadedValue> value = GetValue();

		std::vector<IDagThreadedNode*> array_input_stack = {};
		{
			std::unique_lock read_lock(_array_input_stack_mutex);
			array_input_stack = _array_input_stack;
		}

		std::vector<IDagThreadedNode*> array_input_index = {};
		{
			std::unique_lock read_lock(_array_input_index_mutex);
			array_input_index = _array_input_index;
		}

		DagTooltipDataSource data_source(
			in_collection,
			value,
			_array_input_stack,
			_array_input_index
			);

		_tooltip = TooltipBuilder::BuildTooltip(
			_tooltip_locale_key_text,
			_tooltip_locale_key_children,
			in_locale_system,
			in_locale,
			data_source
			);

		_tooltip_is_null = (nullptr == _tooltip);
	}

	return _tooltip;
}

const bool DagThreadedNodeCalculate::Visit(IDagThreadedVisitor& visitor)
{
	{
		std::unique_lock read_lock_stack(_array_input_stack_mutex);
		std::unique_lock read_lock_index(_array_input_index_mutex);
		std::unique_lock read_lock_output(_array_output_mutex);
		if (false == visitor.OnCalculate(
			GetValue(),
			_calculate_id.load(), // this is not perfect, need to have GetValue return the calculate id?
			_uid,
			_tooltip_locale_key_text,
			_tooltip_locale_key_children,
			_array_input_stack,
			_array_input_index,
			_array_output
			))
		{
			return false;
		}
	}

	{
		std::unique_lock read_lock(_array_input_stack_mutex);
		for (const auto& item : _array_input_stack)
		{
			if (item)
			{
				if (false == item->Visit(visitor))
				{
					return false;
				}
			}
		}
	}

	{
		std::unique_lock read_lock(_array_input_index_mutex);
		for (const auto& item : _array_input_index)
		{
			if (item)
			{
				if (false == item->Visit(visitor))
				{
					return false;
				}
			}
		}
	}

	return true;
}
