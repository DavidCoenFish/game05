#pragma once

#include "common/dag_threaded/i_dag_threaded_node.h"

class IDagThreadedNode;
class IDagThreadedValue;

class DagThreadedVariable : public IDagThreadedNode
{
public:
	DagThreadedVariable();
	virtual ~DagThreadedVariable();

private:
	void SetOutput(IDagThreadedNode* const pNode) override;
	void RemoveOutput(IDagThreadedNode* const pNode) override;
	void AddInputStack(IDagThreadedNode* const pNode) override;
	void RemoveInputStack(IDagThreadedNode* const pNode) override;
	void SetInputIndex(IDagThreadedNode* const pNode, const int index) override;
	std::shared_ptr<IDagThreadedValue> GetValue() override;
	void MarkDirty() override;

private:
	//Dag2::DirtyCase m_dirtyCase;
	std::vector< IDagThreadedNode* > _array_output;
	std::shared_ptr< IDagThreadedValue > _value;

};
