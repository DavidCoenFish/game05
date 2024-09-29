#pragma once

#include "common/dag/threaded/dag_threaded.h"
#include "common/dag/threaded/i_dag_threaded_node.h"

class IDagThreadedNode;
class IDagThreadedValue;
class IDagThreadedVisitor;

// Rename DagThreadedNodeVariable
class DagThreadedNodeVariable : public IDagThreadedNode
{
public:
	DagThreadedNodeVariable(
		const std::string& in_uid,
		const std::shared_ptr< IDagThreadedValue >& in_value,
		const DagThreaded::DirtyCase in_dirty_case,
		const std::string& in_display_name
		);
	virtual ~DagThreadedNodeVariable();

	void SetValue(const std::shared_ptr<IDagThreadedValue>& in_value);

private:
	const std::string& GetUid() const override { return _uid; }
	const std::string& GetDisplayName() const override { return _display_name; }
	const std::string& GetTooltipBody() const override;

	void SetOutput(IDagThreadedNode* const in_node) override;
	void RemoveOutput(IDagThreadedNode* const in_node) override;
	std::shared_ptr<IDagThreadedValue> GetValue() override;

    /// return true to continue visiting 
	virtual const bool Visit(IDagThreadedVisitor& visitor) override;

	void MarkDirty() override;

private:
	const std::string _uid;
	const std::string _display_name;

	const DagThreaded::DirtyCase _dirty_case;

    std::shared_mutex _array_output_mutex;
	std::vector< IDagThreadedNode* > _array_output;

    // Shared_ptr has an internal lock
	std::shared_ptr< IDagThreadedValue > _value;

};
