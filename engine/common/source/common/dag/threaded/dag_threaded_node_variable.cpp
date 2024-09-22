#include "common/common_pch.h"

#include "common/dag/threaded/dag_threaded_node_variable.h"
#include "common/dag/threaded/i_dag_threaded_value.h"
#include "common/dag/threaded/i_dag_threaded_visitor.h"

DagThreadedNodeVariable::DagThreadedNodeVariable(
	const std::string& in_uid,
	const std::shared_ptr< IDagThreadedValue >& in_value,
	const DagThreaded::DirtyCase in_dirty_case,
	const std::string& in_display_name,
	const std::string& in_display_description
	)
	: _uid(in_uid)
    , _display_name(in_display_name)
    , _display_description(in_display_description)
	, _value(in_value)
	, _dirty_case(in_dirty_case)

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

std::shared_ptr<IDagThreadedValue> DagThreadedNodeVariable::GetValue()
{
	return _value;
}

const bool DagThreadedNodeVariable::Visit(IDagThreadedVisitor& visitor) 
{
    return visitor.OnVariable(
        _value,
        _uid,
        _display_name,
        _display_description
        );
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
