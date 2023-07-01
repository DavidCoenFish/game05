#pragma once

#include "common/dag_threaded/i_dag_threaded_node.h"

class IDagThreadedValue;
class DagThreadedCollection;

class DagThreadedCalculate : public IDagThreadedNode
{
public:
	typedef std::function< void (
		const DagThreadedCollection& in_collection,
		std::shared_ptr< IDagThreadedValue >& in_out_value,
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_stack, 
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
		) > CalculateFunction;

	DagThreadedCalculate(const DagThreadedCollection& in_out_collection, const CalculateFunction& in_calculate_function);
	virtual ~DagThreadedCalculate();

private:
	void SetOutput(IDagThreadedNode* const pNode) override;
	void RemoveOutput(IDagThreadedNode* const pNode) override;
	void AddInputStack(IDagThreadedNode* const pNode) override;
	void RemoveInputStack(IDagThreadedNode* const pNode) override;
	void SetInputIndex(IDagThreadedNode* const pNode, const int index) override;
	std::shared_ptr<IDagThreadedValue> GetValue() override;
	void MarkDirty() override;

private:
	const DagThreadedCollection& _collection;
	const CalculateFunction _calculate_function;
	std::vector< IDagThreadedNode* > _array_input_stack;
	std::vector< IDagThreadedNode* > _array_input_index;
	std::vector< IDagThreadedNode* > _array_output;

	std::mutex _value_mutex;
	std::shared_ptr< IDagThreadedValue > _value;

	// Each time we are marked dirty, _change_id increments
	std::atomic_int _change_id;
	// If _calculate_id == _change_id, presume _value is up to date and doesn't need to be calculated via the _calculate_function
	std::atomic_int _calculate_id;
};
