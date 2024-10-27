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
		const std::string& in_tooltip_locale_key_text,
		const std::string& in_tooltip_locale_key_children
		);
	virtual ~DagThreadedNodeVariable();

	void SetValue(const std::shared_ptr<IDagThreadedValue>& in_value);

private:
	virtual const std::string& GetUid() const override;
	virtual void SetOutput(IDagThreadedNode* const in_node) override;
	virtual void RemoveOutput(IDagThreadedNode* const in_node) override;
	virtual void MarkDirty() override;
	virtual std::shared_ptr<IDagThreadedValue> GetValue() override;
	virtual std::shared_ptr<ITooltip> GetTooltip(const DagThreadedCollection& in_collection, const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale) override;
	virtual const bool Visit(IDagThreadedVisitor& visitor) override;

private:
	const std::string _uid;

	const std::string _tooltip_locale_key_text;
	/// a variable child tooltip would normally be empty, but there is no reason for that to always be the case
	const std::string _tooltip_locale_key_children;

	const DagThreaded::DirtyCase _dirty_case;

	std::shared_mutex _array_output_mutex;
	std::vector< IDagThreadedNode* > _array_output;

	/// Shared_ptr has an internal lock
	std::shared_ptr< IDagThreadedValue > _value;

	/// Shared_ptr has an internal lock
	std::shared_ptr<ITooltip> _tooltip;
	/// help not regenerating tooltip on tooltip being null
	bool _tooltip_is_null;


};
