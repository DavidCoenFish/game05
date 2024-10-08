#pragma once

#include "common/dag/threaded/i_dag_threaded_node.h"

class IDagThreadedValue;
class IDagThreadedVisitor;
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
		const std::string& in_uid,
		const CalculateFunction& in_calculate_function,
		const std::string& in_display_name,
		const std::string& in_tooltip_raw
		);
	virtual ~DagThreadedNodeCalculate();

	void AddInputStack(IDagThreadedNode* const pNode);
	void RemoveInputStack(IDagThreadedNode* const pNode);
	void SetInputIndex(IDagThreadedNode* const pNode, const int index);
	void Unlink();

private:
	const std::string& GetUid() const override { return _uid; }
	const std::string& GetDisplayName() const override { return _display_name; }
	const std::string& GetTooltipRaw() const override { return _tooltip_raw; }

	void SetOutput(IDagThreadedNode* const pNode) override;
	void RemoveOutput(IDagThreadedNode* const pNode) override;
	std::shared_ptr<IDagThreadedValue> GetValue() override;

	std::shared_ptr<Tooltip> GetTooltip(const DagThreadedCollection& in_collection, const LocaleSystem& in_locale_system) override;

	/// return true to continue visiting 
	virtual const bool Visit(IDagThreadedVisitor& visitor) override;

	void MarkDirty() override;

private:
	const std::string _uid;
	const std::string _display_name;
	const std::string _tooltip_raw;
	const CalculateFunction _calculate_function;

	std::shared_mutex _array_input_stack_mutex;
	std::vector<IDagThreadedNode*> _array_input_stack;

	std::shared_mutex _array_input_index_mutex;
	std::vector<IDagThreadedNode*> _array_input_index;

	std::shared_mutex _array_output_mutex;
	std::vector<IDagThreadedNode*> _array_output;

	// Each time we are marked dirty, _change_id increments
	std::atomic_int _change_id;

	// If _calculate_id == _change_id, presume _value is up to date and doesn't need to be calculated via the _calculate_function
	std::atomic_int _calculate_id;
	// Shared_ptr has an internal lock
	std::shared_ptr<IDagThreadedValue> _value;

	// If _tooltip_id == _change_id, presume _tooltip is up to date and doesn't need to be calculated
	std::atomic_int _tooltip_id;
	// Shared_ptr has an internal lock
	std::shared_ptr<Tooltip> _tooltip;

};
