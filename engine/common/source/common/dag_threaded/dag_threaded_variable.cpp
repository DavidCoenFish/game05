#include "common/common_pch.h"

#include "common/dag_threaded/dag_threaded_variable.h"
#include "common/dag_threaded/i_dag_threaded_value.h"

DagThreadedVariable::DagThreadedVariable(
	const std::string& in_name,
	const DagThreaded::DirtyCase in_dirty_case
	)
	: _name(in_name)
	, _dirty_case(in_dirty_case)
{
	//nop
}

DagThreadedVariable::~DagThreadedVariable()
{
	//nop
}

void DagThreadedVariable::SetValue(const std::shared_ptr<IDagThreadedValue>& in_value)
{
	bool mark_dirty = false;
	{
		std::lock_guard lock(_value_mutex);
		
		if (0 != (_dirty_case & DagThreaded::DirtyCase::ValueChanged))
		{
			if (nullptr != _value)
			{
				mark_dirty = false == _value->Comparison(in_value.get());
			} 
			else if (nullptr != in_value)
			{
				mark_dirty = true;
			}
		}

		_value = in_value;

		if (nullptr != _value)
		{
			if ((false == mark_dirty) && 
				(0 != (_dirty_case & DagThreaded::DirtyCase::NewValueNotZero)))
			{
				mark_dirty = (false == _value->IsZero());
			}
		}
	}

	if (true == mark_dirty)
	{
		MarkDirty();
	}

	return;
}

void DagThreadedVariable::SetOutput(IDagThreadedNode* const in_node)
{
	if (nullptr != in_node)
	{
		_array_output.push_back(in_node);
	}
	return;
}

void DagThreadedVariable::RemoveOutput(IDagThreadedNode* const in_node)
{
	_array_output.erase(std::remove(_array_output.begin(), _array_output.end(), in_node), _array_output.end());
	return;
}

std::shared_ptr<IDagThreadedValue> DagThreadedVariable::GetValue()
{
	std::shared_ptr<IDagThreadedValue> result;
	{
		std::lock_guard lock(_value_mutex);
		result = _value;
	}
	return result;
}

void DagThreadedVariable::MarkDirty()
{
	for (auto pIter : _array_output)
	{
		pIter->MarkDirty();
	}
	return;
}
