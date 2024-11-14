#include "common/common_pch.h"
#include "common/dag/threaded/dag_threaded_node_variable.h"

#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_value.h"
#include "common/dag/threaded/i_dag_threaded_value.h"
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
			const std::shared_ptr<IDagThreadedValue>& in_value
			)
			: _collection(in_collection)
			, _value(in_value)
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
			return TValue(dag_value_string->Get());
		}

		return TValue();
	}

	virtual const std::string GetText(
		const std::string& in_token, 
		const LocaleSystem& in_locale_system, 
		const LocaleISO_639_1 in_locale) override
	{
		std::string result = {};
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
		DagThreadedCollection::NodeID node_id =_collection.FindNode(in_token);
		std::shared_ptr<ITooltip> tooltip = _collection.GetTooltip(node_id, in_locale_system, in_locale);
		if (nullptr != tooltip)
		{
			in_out_children.push_back(tooltip->Clone(in_clone_verbosity));
		}
	}

	private:
		const DagThreadedCollection& _collection;
		std::shared_ptr<IDagThreadedValue> _value;

	};
}


DagThreadedNodeVariable::DagThreadedNodeVariable(
	const std::string& in_uid,
	const std::shared_ptr< IDagThreadedValue >& in_value,
	const DagThreaded::DirtyCase in_dirty_case,
	const std::string& in_tooltip_locale_key_text,
	const std::string& in_tooltip_locale_key_children
	)
	: _uid(in_uid)
	, _tooltip_locale_key_text(in_tooltip_locale_key_text)
	, _tooltip_locale_key_children(in_tooltip_locale_key_children)
	, _value(in_value)
	, _dirty_case(in_dirty_case)
	, _tooltip_is_null(false)
{
	//nop
}

DagThreadedNodeVariable::~DagThreadedNodeVariable()
{
	//nop
}

void DagThreadedNodeVariable::SetValue(const std::shared_ptr<IDagThreadedValue>& in_value)
{
	bool mark_dirty = false;
	{
		{
			//std::shared_lock write_lock(_value_mutex);
			if (0 != (_dirty_case & DagThreaded::DirtyCase::ValueChanged))
			{
				if ((nullptr == _value) != (nullptr == in_value))
				{
					mark_dirty = true;
				}
				else if (nullptr != _value)
				{
					mark_dirty = false == _value->Comparison(in_value.get());
				} 
			}

			_value = in_value;
		}

		if ((nullptr != in_value) &&
			(false == mark_dirty) && 
			(0 != (_dirty_case & DagThreaded::DirtyCase::NewValueNotZero)))
		{
			mark_dirty = (false == in_value->IsZero());
		}
	}

	if (true == mark_dirty)
	{
		MarkDirty();
	}

	return;
}

const std::string& DagThreadedNodeVariable::GetUid() const
{
	return _uid;
}

void DagThreadedNodeVariable::SetOutput(IDagThreadedNode* const in_node)
{
	if (nullptr != in_node)
	{
		std::shared_lock write_lock(_array_output_mutex);
		_array_output.push_back(in_node);
	}
	return;
}

void DagThreadedNodeVariable::RemoveOutput(IDagThreadedNode* const in_node)
{
	std::shared_lock write_lock(_array_output_mutex);
	_array_output.erase(std::remove(_array_output.begin(), _array_output.end(), in_node), _array_output.end());
	return;
}

void DagThreadedNodeVariable::MarkDirty()
{
	std::unique_lock read_lock(_array_output_mutex);
	for (auto pIter : _array_output)
	{
		pIter->MarkDirty();
	}
	return;
}

std::shared_ptr<IDagThreadedValue> DagThreadedNodeVariable::GetValue()
{
	return _value;
}

std::shared_ptr<ITooltip> DagThreadedNodeVariable::GetTooltip(const DagThreadedCollection& in_collection, const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale)
{
	if ((false == _tooltip_locale_key_text.empty()) && (false == _tooltip_is_null) && (nullptr == _tooltip))
	{
		std::shared_ptr<IDagThreadedValue> value = GetValue();

		DagTooltipDataSource data_source(
			in_collection,
			value
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

const bool DagThreadedNodeVariable::Visit(IDagThreadedVisitor& visitor)
{
	return visitor.OnVariable(
		_value,
		_uid,
		_tooltip_locale_key_text,
		_tooltip_locale_key_children
		);
}
