#pragma once

#include "common/dag_threaded/i_dag_threaded_node.h"

class IDagThreadedValue;
class DagThreadedCollection;

// Rename DagThreadedNodeCalculate
class DagThreadedNodeCalculate : public IDagThreadedNode
{
public:
	typedef std::function< std::shared_ptr< IDagThreadedValue > (
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_stack, 
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
		) > CalculateFunction;

	DagThreadedNodeCalculate(
		//const DagThreadedCollection& in_collection, 
		const std::string& in_name,
		const CalculateFunction& in_calculate_function
		);
	virtual ~DagThreadedNodeCalculate();

	void AddInputStack(IDagThreadedNode* const pNode);
	void RemoveInputStack(IDagThreadedNode* const pNode);
	void SetInputIndex(IDagThreadedNode* const pNode, const int index);
	void Unlink();

private:
	const std::string& GetName() const override { return _name; }
    //std::mutex& GetOutputMutex() override { return _array_output_mutex; }
	void SetOutput(IDagThreadedNode* const pNode) override;
	void RemoveOutput(IDagThreadedNode* const pNode) override;
	std::shared_ptr<IDagThreadedValue> GetValue() override;
	void MarkDirty() override;

private:
	//const DagThreadedCollection& _collection;
	const std::string _name;
	const CalculateFunction _calculate_function;

    std::shared_mutex _array_input_stack_mutex;
    std::vector< IDagThreadedNode* > _array_input_stack;

    std::shared_mutex _array_input_index_mutex;
    std::vector< IDagThreadedNode* > _array_input_index;

    std::shared_mutex _array_output_mutex;
    std::vector< IDagThreadedNode* > _array_output;

    // Shared_ptr has an internal lock
    //std::shared_mutex _value_mutex;
	std::shared_ptr< IDagThreadedValue > _value;

	// Each time we are marked dirty, _change_id increments
	std::atomic_int _change_id;
	// If _calculate_id == _change_id, presume _value is up to date and doesn't need to be calculated via the _calculate_function
	std::atomic_int _calculate_id;
};
