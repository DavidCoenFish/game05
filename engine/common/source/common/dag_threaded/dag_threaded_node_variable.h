#pragma once

#include "common/dag_threaded/dag_threaded.h"
#include "common/dag_threaded/i_dag_threaded_node.h"

class IDagThreadedNode;
class IDagThreadedValue;

// Rename DagThreadedNodeVariable
class DagThreadedNodeVariable : public IDagThreadedNode
{
public:
	DagThreadedNodeVariable(
		const std::string& in_name, 
		const std::shared_ptr< IDagThreadedValue >& in_value,
		const DagThreaded::DirtyCase in_dirty_case
		);
	virtual ~DagThreadedNodeVariable();

	void SetValue(const std::shared_ptr<IDagThreadedValue>& in_value);

private:
	const std::string& GetName() const override { return _name; }
    //std::mutex& GetOutputMutex() override { return _array_output_mutex; }
	void SetOutput(IDagThreadedNode* const in_node) override;
	void RemoveOutput(IDagThreadedNode* const in_node) override;
	std::shared_ptr<IDagThreadedValue> GetValue() override;
	void MarkDirty() override;

private:
	//const DagThreadedCollection& _collection;
	const DagThreaded::DirtyCase _dirty_case;
	const std::string _name;

    std::shared_mutex _array_output_mutex;
	std::vector< IDagThreadedNode* > _array_output;

    // Shared_ptr has an internal lock
	//std::mutex _value_mutex; 
	std::shared_ptr< IDagThreadedValue > _value;

};
