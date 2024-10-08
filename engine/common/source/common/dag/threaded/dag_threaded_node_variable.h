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
		const std::string& in_display_name,
		const std::string& in_tooltip_raw
		);
	virtual ~DagThreadedNodeVariable();

	void SetValue(const std::shared_ptr<IDagThreadedValue>& in_value);

private:
	const std::string& GetUid() const override { return _uid; }
	const std::string& GetDisplayName() const override { return _display_name; }
	const std::string& GetTooltipRaw() const override { return _tooltip_raw; }
	void SetOutput(IDagThreadedNode* const in_node) override;
	void RemoveOutput(IDagThreadedNode* const in_node) override;
	std::shared_ptr<IDagThreadedValue> GetValue() override;
	std::shared_ptr<Tooltip> GetTooltip(const DagThreadedCollection& in_collection, const LocaleSystem& in_locale_system) override;

	/// return true to continue visiting 
	virtual const bool Visit(IDagThreadedVisitor& visitor) override;

	void MarkDirty() override;

private:
	const std::string _uid;
	const std::string _display_name;
	const std::string _tooltip_raw;

	const DagThreaded::DirtyCase _dirty_case;

	std::shared_mutex _array_output_mutex;
	std::vector< IDagThreadedNode* > _array_output;

	// Shared_ptr has an internal lock
	std::shared_ptr< IDagThreadedValue > _value;

};
