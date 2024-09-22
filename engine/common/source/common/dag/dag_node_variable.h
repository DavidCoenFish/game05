#pragma once

#include "common/dag/dag.h"
#include "common/dag/i_dag_node.h"

class IDagNode;
class IDagValue;

// Rename DagNodeVariable
class DagNodeVariable : public IDagNode
{
public:
	DagNodeVariable(
		const std::string& in_name, 
		const std::shared_ptr< IDagValue >& in_value,
		const Dag::DirtyCase in_dirty_case
		);
	virtual ~DagNodeVariable();

	void SetValue(const std::shared_ptr<IDagValue>& in_value);

private:
	const std::string& GetName() const override { return _name; }
	void SetOutput(IDagNode* const in_node) override;
	void RemoveOutput(IDagNode* const in_node) override;
	std::shared_ptr<IDagValue> GetValue() override;
	void MarkDirty() override;

private:
	const Dag::DirtyCase _dirty_case;
	const std::string _name;

	std::vector< IDagNode* > _array_output;

	std::shared_ptr< IDagValue > _value;

};
