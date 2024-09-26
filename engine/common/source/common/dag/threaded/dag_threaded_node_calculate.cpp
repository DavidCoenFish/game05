#include "common/common_pch.h"

#include "common/dag/threaded/dag_threaded_node_calculate.h"
#include "common/dag/threaded/i_dag_threaded_visitor.h"


DagThreadedNodeCalculate::DagThreadedNodeCalculate(
	const std::string& in_uid,
	const CalculateFunction& in_calculate_function,
	const std::string& in_display_name
    )
	: _uid(in_uid)
    , _display_name(in_display_name)
	, _calculate_function(in_calculate_function)
{
	//nop
}

DagThreadedNodeCalculate::~DagThreadedNodeCalculate()
{
	//nop
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

std::shared_ptr<IDagThreadedValue> DagThreadedNodeCalculate::GetValue()
{
	const int change_id = _change_id.load();
	const int calculate_id = _calculate_id.exchange(change_id);
	std::shared_ptr<IDagThreadedValue> result;
	{
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
        
		result = _value;
	}
	return result;
}

const bool DagThreadedNodeCalculate::Visit(IDagThreadedVisitor& visitor)
{
    {
        std::unique_lock read_lock_stack(_array_input_stack_mutex);
        std::unique_lock read_lock_index(_array_input_index_mutex);
        std::unique_lock read_lock_output(_array_output_mutex);
        if (false == visitor.OnCalculate(
            GetValue(),
            _uid,
            _display_name,
            _array_input_index,
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
