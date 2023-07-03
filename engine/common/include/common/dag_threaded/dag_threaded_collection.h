#pragma once

#include "common/dag_threaded/dag_threaded.h"
#include "common/dag_threaded/dag_threaded_value.h"

class DagThreadedCollectionImplementation;
class IDagThreadedValue;

class DagThreadedCollection
{
public:
	typedef void* NodeID;

	typedef std::function< std::shared_ptr< IDagThreadedValue > (
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_stack, 
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
		) > CalculateFunction;

	static std::shared_ptr<DagThreadedCollection> Factory();
	DagThreadedCollection();
	~DagThreadedCollection();

	// true: get/set value allowed on all threads, add/remove links/nodes are prohibited
	// false: get/set value, add/remove links/nodes allowed
	void SetMultiThreadedMode(const bool in_enabled);

	// Primary thread, multi threaded mode == false
	NodeID FindNode(const std::string& in_name);
	NodeID CreateNodeVariable(
		const std::string& in_name, 
		const std::shared_ptr< IDagThreadedValue >& in_dag_value = nullptr,
		const DagThreaded::DirtyCase in_dirty_case = DagThreaded::DirtyCase::ValueChanged
		);
	NodeID CreateNodeCalculate(const std::string& in_name, const CalculateFunction& in_function);
	void DestroyNode(const NodeID in_node_id);
	void AddNodeLinkIndexed(const NodeID in_node_id_subject, const NodeID in_node_id_to_add, const int in_index);
	void RemoveNodeLinkIndexed(const NodeID in_node_id_subject, const int in_index);
	void AddNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_add);
	void RemoveNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_remove);
	void UnlinkNode(const NodeID in_node_id);

	// Primary thread unless multi threaded mode == true
	std::shared_ptr< IDagThreadedValue > GetDagValue(const NodeID in_node_id);
	void SetDagValue(const NodeID in_node_id, const std::shared_ptr< IDagThreadedValue >& in_dag_value);

	//template <typename TYPE>
	//const std::shared_ptr<IDagThreadedValue> CreateDagValue(const TYPE in_value)
	//{
	//	CheckPrimaryThreadOrMultiThreadedMode();
	//	return std::make_shared< DagThreadedValue< TYPE > >(in_value);
	//}

private:
	void CheckPrimaryThreadOrMultiThreadedMode() const;

private:
	std::unique_ptr< DagThreadedCollectionImplementation > _implementation;

};
