#pragma once

#include "common/dag/i_dag_node.h"

class IDagValue;
class DagCollection;

class DagNodeCalculate : public IDagNode
{
public:
	typedef std::function< std::shared_ptr< IDagValue > (
		const std::vector< std::shared_ptr< IDagValue > >& in_array_stack, 
		const std::vector< std::shared_ptr< IDagValue > >& in_array_indexed
		) > CalculateFunction;

	DagNodeCalculate(
		const std::string& in_name,
		const CalculateFunction& in_calculate_function
		);
	virtual ~DagNodeCalculate();

	void AddInputStack(IDagNode* const pNode);
	void RemoveInputStack(IDagNode* const pNode);
	void SetInputIndex(IDagNode* const pNode, const int index);
	void Unlink();

private:
	const std::string& GetName() const override { return _name; }
	void SetOutput(IDagNode* const pNode) override;
	void RemoveOutput(IDagNode* const pNode) override;
	std::shared_ptr<IDagValue> GetValue() override;
	void MarkDirty() override;

private:
	const std::string _name;
	const CalculateFunction _calculate_function;

    std::vector< IDagNode* > _array_input_stack;

    std::vector< IDagNode* > _array_input_index;

    std::vector< IDagNode* > _array_output;

    // Shared_ptr has an internal lock
	std::shared_ptr< IDagValue > _value;

	// Each time we are marked dirty, _change_id increments
	std::atomic_int _change_id;
	// If _calculate_id == _change_id, presume _value is up to date and doesn't need to be calculated via the _calculate_function
	std::atomic_int _calculate_id;
};
