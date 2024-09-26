#pragma once

#include "common/dag/threaded/dag_threaded.h"
#include "common/dag/threaded/dag_threaded_value.h"

class DagThreadedCollectionImplementation;
class IDagThreadedNode;
class IDagThreadedValue;
class IDagThreadedVisitor;

/*
"foo(7) = bar(3) + meow(4)", where bar(3) is a link to bar tooltip, if a calculate, and meow(4) a link to 


for node foo
DagNode::GetTooltipName() "foo" // locale key?
DagNode::GetTooltipText() "foo(7)" // generated from tooltip name and node value
DagNode::GetTooltipBody() "{self} = {index.0} + {index.1}" => "foo(7) = bar(3) + meow(4)" // locale key?

for node bar
DagNode::GetTooltipName() "bar"
DagNode::GetTooltipText() "bar(3)"
DagNode::GetTooltipBody() "" // node was a value

for node meow
DagNode::GetTooltipName() "meow"
DagNode::GetTooltipText() "meow(4)"
DagNode::GetTooltipBody() "meow(4) = fred(2) * barney(2)" // node was a calculate

*/
class DagThreadedCollection
{
public:
	//typedef void* NodeID;
    typedef IDagThreadedNode* NodeID;

	typedef std::function< std::shared_ptr< IDagThreadedValue > (
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_stack, 
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
		) > CalculateFunction;

	static std::shared_ptr<DagThreadedCollection> Factory();
	DagThreadedCollection();
	~DagThreadedCollection();

	// Primary thread, multi threaded mode == false
	NodeID FindNode(const std::string& in_uid);
    /// rename "name" as uid? id?
	NodeID CreateNodeVariable(
		const std::string& in_uid, 
		const std::shared_ptr< IDagThreadedValue >& in_dag_value = nullptr,
		const DagThreaded::DirtyCase in_dirty_case = DagThreaded::DirtyCase::ValueChanged,
        const std::string& in_display_name = "" // possibly a local key
		);
	NodeID CreateNodeCalculate(
        const std::string& in_uid, 
        const CalculateFunction& in_function,
        const std::string& in_display_name = "" // possibly a local key
        );
	void DestroyNode(const NodeID in_node_id);
	void AddNodeLinkIndexed(const NodeID in_node_id_subject, const NodeID in_node_id_to_add, const int in_index);
	void RemoveNodeLinkIndexed(const NodeID in_node_id_subject, const int in_index);
	void AddNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_add);
	void RemoveNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_remove);
	void UnlinkNode(const NodeID in_node_id);

    /// here or in helper
    const bool VisitNode(const NodeID in_node_id, IDagThreadedVisitor& in_visitor);

	// Primary thread unless multi threaded mode == true
	std::shared_ptr< IDagThreadedValue > GetDagValue(const NodeID in_node_id);
	void SetDagValue(const NodeID in_node_id, const std::shared_ptr< IDagThreadedValue >& in_dag_value);

private:
	std::unique_ptr< DagThreadedCollectionImplementation > _implementation;

};
