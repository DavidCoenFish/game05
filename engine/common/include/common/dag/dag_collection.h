#pragma once

#include "common/dag/dag.h"

class DagCollectionImplementation;
class IDagValue;
class IDagNode;
class Tooltip;

class DagCollection
{
public:
	typedef void* NodeID;
    //typedef std::shared_ptr< IDagNode > NodeID;

	typedef std::function< std::shared_ptr< IDagValue > (
		const std::vector< std::shared_ptr< IDagValue > >& in_array_stack, 
		const std::vector< std::shared_ptr< IDagValue > >& in_array_indexed
		) > CalculateFunction;

	static std::shared_ptr<DagCollection> Factory();
	DagCollection();
	~DagCollection();

	NodeID FindNode(const std::string& in_name);
	NodeID CreateNodeVariable(
		const std::string& in_name, 
		const std::shared_ptr< IDagValue >& in_dag_value = nullptr,
		const Dag::DirtyCase in_dirty_case = Dag::DirtyCase::ValueChanged
		);
	NodeID CreateNodeCalculate(const std::string& in_name, const CalculateFunction& in_function);
	void DestroyNode(const NodeID& in_node_id);
	void AddNodeLinkIndexed(const NodeID& in_node_id_subject, const NodeID& in_node_id_to_add, const int in_index);
	void RemoveNodeLinkIndexed(const NodeID& in_node_id_subject, const int in_index);
	void AddNodeLinkStack(const NodeID& in_node_id_subject, const NodeID& in_node_id_to_add);
	void RemoveNodeLinkStack(const NodeID& in_node_id_subject, const NodeID& in_node_id_to_remove);
	void UnlinkNode(const NodeID& in_node_id);

	std::shared_ptr< IDagValue > GetDagValue(const NodeID& in_node_id);
	void SetDagValue(const NodeID& in_node_id, const std::shared_ptr< IDagValue >& in_dag_value);

    std::shared_ptr<Tooltip> MakeTooltip(const NodeID& in_node_id);

private:
	std::unique_ptr< DagCollectionImplementation > _implementation;

};
