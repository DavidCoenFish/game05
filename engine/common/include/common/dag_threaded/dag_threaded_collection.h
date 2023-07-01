#pragma once

class DagThreadedCollectionImplementation;
class IDagThreadedValue;

class DagThreadedCollection
{
public:
	typedef void* NodeID;
	enum DirtyCase
	{
		None = 0,
		ValueChanged = 1 << 0,
		NewValueNotZero = 1 << 1
	};
	typedef std::function< void (
		const DagThreadedCollection& in_collection,
		std::shared_ptr< IDagThreadedValue >& in_out_value,
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_stack, 
		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
		) > CalculateFunction;

	static std::shared_ptr<DagThreadedCollection> Factory();
	DagThreadedCollection();
	~DagThreadedCollection();

	// true: get/set value allowed on all threads, add/remove links/nodes are prohibited
	// false: get/set value, add/remove links/nodes allowed
	void SetMultiThreadedMode(const bool in_enabled);

	NodeID FindNode(const std::string& in_name);
	NodeID CreateNodeVariable(const std::string& in_name, const DirtyCase in_dirty_case = DirtyCase::ValueChanged);
	NodeID CreateNodeCalculate(const std::string& in_name, const CalculateFunction& in_function);
	void DestroyNode(const NodeID in_node_id);
	std::shared_ptr< IDagThreadedValue > GetNodeValue(const NodeID in_node_id);

	void AddNodeLinkIndexed(const NodeID in_node_id_subject, const NodeID in_node_id_to_add, const int in_index);
	void RemoveNodeLinkIndexed(const NodeID in_node_id_subject, const int in_index);

	void AddNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_add);
	void RemoveNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_remove);

	void UnlinkNode(const NodeID in_node_id);

	//template <typename TYPE>
	//void SetValue(const std::shared_ptr< IDagThreadedValue >& node, const TYPE in_value);
	//template <typename TYPE>
	//const TYPE GetValue(const std::shared_ptr< IDagThreadedValue >& node);
	//template <typename TYPE>
	//void SetValueRef(const std::shared_ptr< IDagThreadedValue >& node, const TYPE& in_value);
	//template <typename TYPE>
	//const TYPE& GetValueRef(const std::shared_ptr< IDagThreadedValue >& node);

private:
	std::unique_ptr< DagThreadedCollectionImplementation > _implementation;

};
