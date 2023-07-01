#include "common/common_pch.h"

#include "common/dag_threaded/dag_threaded_calculate.h"


DagThreadedCalculate::DagThreadedCalculate(const DagThreadedCollection& in_collection, const CalculateFunction& in_calculate_function)
	: _collection(in_collection)
	, _calculate_function(in_calculate_function)
{
	//nop
}

DagThreadedCalculate::~DagThreadedCalculate()
{
	//nop
}

void DagThreadedCalculate::SetOutput(IDagThreadedNode* const in_node)
{
	in_node;
}

void DagThreadedCalculate::RemoveOutput(IDagThreadedNode* const in_node)
{
	in_node;
}

void DagThreadedCalculate::AddInputStack(IDagThreadedNode* const in_node)
{
	in_node;
}

void DagThreadedCalculate::RemoveInputStack(IDagThreadedNode* const in_node)
{
	in_node;
}

void DagThreadedCalculate::SetInputIndex(IDagThreadedNode* const in_node, const int in_index)
{
	in_node;
	in_index;
}

std::shared_ptr<IDagThreadedValue> DagThreadedCalculate::GetValue()
{
	const int change_id = _change_id.load();
	const int calculate_id = _calculate_id.exchange(change_id);
	std::shared_ptr<IDagThreadedValue> result;

	{
		std::lock_guard lock(_value_mutex);
		if ((change_id != calculate_id) && (nullptr != _calculate_function))
		{
			std::vector< std::shared_ptr< IDagThreadedValue > > array_stack;
			for(auto iter: _array_input_stack)
			{
				auto value = iter ? iter->GetValue() : nullptr;
				array_stack.push_back(value);
			}

			std::vector< std::shared_ptr< IDagThreadedValue > > array_indexed;
			for(auto iter: _array_input_stack)
			{
				auto value = iter ? iter->GetValue() : nullptr;
				array_indexed.push_back(value);
			}

			_calculate_function(_collection, _value, array_stack, array_indexed);
		}
		result = _value;
	}
	return result;
}

void DagThreadedCalculate::MarkDirty()
{
	const int before_add_change_id = _change_id.fetch_add(1);
	const int calculate_id = _calculate_id.load();

	// only need to tell outputs we became dirty again when we change to be dirty
	if (before_add_change_id == calculate_id)
	{
		for (auto pIter : _array_output)
		{
			pIter->MarkDirty();
		}
	}
}
