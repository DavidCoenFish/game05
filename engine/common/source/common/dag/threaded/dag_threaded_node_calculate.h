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
		const std::string& in_tooltip_locale_key_text,
		const std::string& in_tooltip_locale_key_children
		);
	virtual ~DagThreadedNodeCalculate();

	void AddInputStack(IDagThreadedNode* const pNode);
	void RemoveInputStack(IDagThreadedNode* const pNode);
	void SetInputIndex(IDagThreadedNode* const pNode, const int index);
	void Unlink();

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
	const CalculateFunction _calculate_function;

	const std::string _tooltip_locale_key_text;
	const std::string _tooltip_locale_key_children;

	std::shared_mutex _array_input_stack_mutex;
	std::vector<IDagThreadedNode*> _array_input_stack;

	std::shared_mutex _array_input_index_mutex;
	std::vector<IDagThreadedNode*> _array_input_index;

	std::shared_mutex _array_output_mutex;
	std::vector<IDagThreadedNode*> _array_output;

	/// Each time we are marked dirty, _change_id increments
	std::atomic_int _change_id;

	/// If _calculate_id == _change_id, presume _value is up to date and doesn't need to be calculated via the _calculate_function
	std::atomic_int _calculate_id;
	/// Shared_ptr has an internal lock
	std::shared_ptr<IDagThreadedValue> _value;

	/// If _tooltip_id == _change_id, presume _tooltip is up to date and doesn't need to be calculated
	std::atomic_int _tooltip_id;
	/// Shared_ptr has an internal lock
	std::shared_ptr<ITooltip> _tooltip;
	/// help not regenerating tooltip on tooltip being null
	bool _tooltip_is_null;

};
