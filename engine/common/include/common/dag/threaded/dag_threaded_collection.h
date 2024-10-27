#pragma once

#include "common/dag/threaded/dag_threaded.h"

class DagThreadedCollectionImplementation;
class IDagThreadedNode;
class IDagThreadedValue;
class IDagThreadedVisitor;
class GetTooltip;
class LocaleSystem;

class DagThreadedCollection
{
public:
	typedef IDagThreadedNode* NodeID;

	typedef std::function< std::shared_ptr< IDagThreadedValue > (
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_stack, 
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
		) > CalculateFunction;

	static std::shared_ptr<DagThreadedCollection> Factory();
	DagThreadedCollection();
	~DagThreadedCollection();

	const std::string MakeUid();

	// Primary thread, multi threaded mode == false
	NodeID FindNode(const std::string& in_uid) const;
	/// rename "name" as uid? id?
	NodeID CreateNodeVariable(
		const std::string& in_uid, 
		const std::shared_ptr< IDagThreadedValue >& in_dag_value = nullptr,
		const DagThreaded::DirtyCase in_dirty_case = DagThreaded::DirtyCase::ValueChanged,
		const std::string& in_display_name = "", // possibly a local key
		const std::string& in_tooltip_raw = "" // possibly a local key
		);
	NodeID CreateNodeCalculate(
		const std::string& in_uid, 
		const CalculateFunction& in_function,
		const std::string& in_display_name = "", // possibly a local key
		const std::string& in_tooltip_raw = "" // possibly a local key
		);
	void DestroyNode(const NodeID in_node_id);
	void AddNodeLinkIndexed(const NodeID in_node_id_subject, const NodeID in_node_id_to_add, const int in_index);
	void RemoveNodeLinkIndexed(const NodeID in_node_id_subject, const int in_index);
	void AddNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_add);
	void RemoveNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_remove);
	void UnlinkNode(const NodeID in_node_id);

	const bool VisitNode(const NodeID in_node_id, IDagThreadedVisitor& in_visitor);

	std::shared_ptr<IDagThreadedValue> GetDagValue(const NodeID in_node_id);
	void SetDagValue(const NodeID in_node_id, const std::shared_ptr<IDagThreadedValue>& in_dag_value);

	/// this could have been kept external to the DagCollection and implemented via visitor, but want to cache the tooltip
	/// and only regenerate if dirty
	std::shared_ptr<ITooltip> GetTooltip(const NodeID in_node_id, const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale) const;

private:
	std::unique_ptr< DagThreadedCollectionImplementation > _implementation;

};
