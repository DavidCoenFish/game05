#pragma once

class DagThreadedCollectionImplementation;
class IDagThreadedValue;

class DagThreadedCollection
{
public:
	typedef void* NodeID;
	//value, array stack, array indexed, 
	typedef std::function< void (
		std::shared_ptr< IDagThreadedValue >&,
		const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
		const std::vector< std::shared_ptr< IDagThreadedValue > >&
		) > CalculateFunction;

	static std::shared_ptr<DagThreadedCollection> Factory();
	DagThreadedCollection();
	~DagThreadedCollection();

	// true: get/set value allowed on all threads, add/remove links/nodes are prohibited
	// false: get/set value, add/remove links/nodes allowed
	void SetMultiThreadedMode(const bool in_enabled);

	//template<typename TYPE>
	//NodeID CreateNodeValue(const std::string& in_name, const TYPE in_value)

	NodeID CreateNodeCalculate(const std::string& in_name, const CalculateFunction& in_function);

	//void DestroyNode(const NodeID in_nodeID);

	//void AddNodeLinkIndex();
	//void RemoveNodeLinkIndex();

	//void AddNodeLinkStack();
	//void RemoveNodeLinkStack();

	//void UnlinkNode(const NodeID in_nodeID);

	//template<typename TYPE>
	//void SetValue(const NodeID in_nodeID, const TYPE in_value)
	//{
	//}

	//template<typename TYPE>
	//const TYPE GetValue(const NodeID in_nodeID)
	//{
	//}

private:
	std::unique_ptr< DagThreadedCollectionImplementation > _implementation;

};
