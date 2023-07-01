#include "common/common_pch.h"
#include "common/dag_threaded/dag_threaded_collection.h"

#include "common/dag_threaded/dag_threaded_calculate.h"
#include "common/dag_threaded/i_dag_threaded_node.h"
#include "common/platform.h"

class DagThreadedCollectionImplementation
{
public:
	DagThreadedCollectionImplementation(const int in_main_thread_id)
		: _main_thread_id(in_main_thread_id)
		, _multi_threaded_mode(false)
	{
		//nop
	}
	~DagThreadedCollectionImplementation()
	{
		//nop
	}
	void CleanUp()
	{
		DSC_CONDITION_THROW(true == _multi_threaded_mode, "_multi_threaded_mode should not be true on clean up");
	}
	void SetMultiThreadedMode(const bool in_enabled)
	{
		_multi_threaded_mode = in_enabled;
	}
	DagThreadedCollection::NodeID CreateNodeCalculate(const DagThreadedCollection& in_collection, const std::string& in_name, const DagThreadedCollection::CalculateFunction& in_function)
	{
		DSC_CONDITION_THROW(true == _multi_threaded_mode, "_multi_threaded_mode should not be true on create node calculate");

		auto node_calculate = std::make_shared<DagThreadedCalculate>(in_collection, in_function);
		_dag_node_map[in_name] = node_calculate;

		return (DagThreadedCollection::NodeID)node_calculate.get();
	}

private:
	const int _main_thread_id;
	bool _multi_threaded_mode;
	std::map< std::string, std::shared_ptr< IDagThreadedNode > > _dag_node_map;

};

std::shared_ptr<DagThreadedCollection> DagThreadedCollection::Factory()
{
	return std::make_shared<DagThreadedCollection>();
}

DagThreadedCollection::DagThreadedCollection()
{
	_implementation = std::make_unique<DagThreadedCollectionImplementation>(Platform::GetCurrentThreadID());
}

DagThreadedCollection::~DagThreadedCollection()
{
	try
	{
		_implementation->CleanUp();
	}
	catch(...)
	{
		DSC_ASSERT("clean up failed" && false);
	}
}

void DagThreadedCollection::SetMultiThreadedMode(const bool in_enabled)
{
	_implementation->SetMultiThreadedMode(in_enabled);
}

DagThreadedCollection::NodeID DagThreadedCollection::CreateNodeCalculate(const std::string& in_name, const CalculateFunction& in_function)
{
	return _implementation->CreateNodeCalculate(*this, in_name, in_function);
}
