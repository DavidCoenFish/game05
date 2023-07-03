#include "common/common_pch.h"
#include "common/dag_threaded/dag_threaded_collection.h"

#include "common/dag_threaded/dag_threaded_calculate.h"
#include "common/dag_threaded/dag_threaded_variable.h"
#include "common/dag_threaded/i_dag_threaded_node.h"
#include "common/platform.h"

class DagThreadedCollectionImplementation
{
public:
	DagThreadedCollectionImplementation()
		: _primary_thread_id(Platform::GetCurrentThreadID())
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
		DSC_CONDITION_THROW(_primary_thread_id != Platform::GetCurrentThreadID(), "CleanUp should only be run from the primary thread")
		//DSC_CONDITION_THROW(true == _multi_threaded_mode, "_multi_threaded_mode should not be true on clean up"); //Why is this a problem?
	}
	void SetMultiThreadedMode(const bool in_enabled)
	{
		_multi_threaded_mode = in_enabled;
	}

	DagThreadedCollection::NodeID FindNode(const std::string& in_name)
	{
		DSC_CONDITION_THROW(_primary_thread_id != Platform::GetCurrentThreadID(), "FindNode should only be run from the primary thread")
		auto found = _dag_node_map.find(in_name);
		if (found != _dag_node_map.end())
		{
			return (DagThreadedCollection::NodeID)found->second.get();
		}
		return (DagThreadedCollection::NodeID)nullptr;
	}

	DagThreadedCollection::NodeID CreateNodeVariable(
		const std::string& in_name, 
		const std::shared_ptr< IDagThreadedValue >& in_dag_value,
		const DagThreaded::DirtyCase in_dirty_case
		)
	{
		CheckPrimaryThreadAndNotMultiThreadedMode();
		DSC_CONDITION_THROW(_dag_node_map.find(in_name) != _dag_node_map.end(), "CreateNodeVariable replacing existing node");

		auto node_variable = std::make_shared<DagThreadedVariable>(in_name, in_dag_value, in_dirty_case);
		_dag_node_map[in_name] = node_variable;

		return (DagThreadedCollection::NodeID)node_variable.get();
	}

	//DagThreadedCollection::NodeID CreateNodeCalculate(const DagThreadedCollection& in_collection, const std::string& in_name, const DagThreadedCollection::CalculateFunction& in_function)
	DagThreadedCollection::NodeID CreateNodeCalculate(const std::string& in_name, const DagThreadedCollection::CalculateFunction& in_function)
	{
		CheckPrimaryThreadAndNotMultiThreadedMode();
		DSC_CONDITION_THROW(_dag_node_map.find(in_name) != _dag_node_map.end(), "CreateNodeCalculate replacing existing node");

		//auto node_calculate = std::make_shared<DagThreadedCalculate>(in_name, in_collection, in_function);
		auto node_calculate = std::make_shared<DagThreadedCalculate>(in_name, in_function);
		_dag_node_map[in_name] = node_calculate;

		return (DagThreadedCollection::NodeID)node_calculate.get();
	}

	void DestroyNode(const DagThreadedCollection::NodeID in_node_id)
	{
		CheckPrimaryThreadAndNotMultiThreadedMode();

		auto node = (IDagThreadedNode*)in_node_id;
		if (nullptr != node)
		{
			auto found = _dag_node_map.find(node->GetName());
			if (found != _dag_node_map.end())
			{
				_dag_node_map.erase(found);
			}
		}
		return;
	}

	void AddNodeLinkIndexed(const DagThreadedCollection::NodeID in_node_id_subject, const DagThreadedCollection::NodeID in_node_id_to_add, const int in_index)
	{
		CheckPrimaryThreadAndNotMultiThreadedMode();
		auto node_subject = dynamic_cast<DagThreadedCalculate*>((IDagThreadedNode*)in_node_id_subject);
		auto node_to_add = (IDagThreadedNode*)in_node_id_to_add;
		if (nullptr != node_subject)
		{
			node_subject->SetInputIndex(node_to_add, in_index);
		}
		return;
	}
	void RemoveNodeLinkIndexed(const DagThreadedCollection::NodeID in_node_id_subject, const int in_index)
	{
		CheckPrimaryThreadAndNotMultiThreadedMode();
		auto node_subject = dynamic_cast<DagThreadedCalculate*>((IDagThreadedNode*)in_node_id_subject);
		if (nullptr != node_subject)
		{
			node_subject->SetInputIndex(nullptr, in_index);
		}
		return;
	}
	void AddNodeLinkStack(const DagThreadedCollection::NodeID in_node_id_subject, const DagThreadedCollection::NodeID in_node_id_to_add)
	{
		CheckPrimaryThreadAndNotMultiThreadedMode();
		auto node_subject = dynamic_cast<DagThreadedCalculate*>((IDagThreadedNode*)in_node_id_subject);
		auto node_to_add = (IDagThreadedNode*)in_node_id_to_add;
		if (nullptr != node_subject)
		{
			node_subject->AddInputStack(node_to_add);
		}
		return;
	}
	void RemoveNodeLinkStack(const DagThreadedCollection::NodeID in_node_id_subject, const DagThreadedCollection::NodeID in_node_id_to_remove)
	{
		CheckPrimaryThreadAndNotMultiThreadedMode();
		auto node_subject = dynamic_cast<DagThreadedCalculate*>((IDagThreadedNode*)in_node_id_subject);
		auto node_to_remove = (IDagThreadedNode*)in_node_id_to_remove;
		if (nullptr != node_subject)
		{
			node_subject->RemoveInputStack(node_to_remove);
		}
	}
	void UnlinkNode(const DagThreadedCollection::NodeID in_node_id)
	{
		CheckPrimaryThreadAndNotMultiThreadedMode();
		auto node_calculate = dynamic_cast<DagThreadedCalculate*>((IDagThreadedNode*)in_node_id);
		if (nullptr != node_calculate)
		{
			node_calculate->Unlink();
		}
		return;
	}

	std::shared_ptr< IDagThreadedValue > GetDagValue(const DagThreadedCollection::NodeID in_node_id)
	{
		CheckPrimaryThreadOrMultiThreadedMode();
		auto node = (IDagThreadedNode*)in_node_id;
		if (nullptr != node)
		{
			return node->GetValue();
		}
		return nullptr;
	}

	void SetDagValue(const DagThreadedCollection::NodeID in_node_id, const std::shared_ptr< IDagThreadedValue >& in_dag_value)
	{
		CheckPrimaryThreadOrMultiThreadedMode();
		auto node_variable = dynamic_cast<DagThreadedVariable*>((IDagThreadedNode*)in_node_id);
		if (nullptr != node_variable)
		{
			node_variable->SetValue(in_dag_value);
		}
		return;
	}

	void CheckPrimaryThreadOrMultiThreadedMode() const
	{
		DSC_CONDITION_THROW((_primary_thread_id != Platform::GetCurrentThreadID()) &&
			(_multi_threaded_mode == false),
			"should only be run from the primary thread or multi threaded mode should be enabled");
		return;
	}

	void CheckPrimaryThreadAndNotMultiThreadedMode() const
	{
		DSC_CONDITION_THROW(_primary_thread_id != Platform::GetCurrentThreadID(), "should only be run from the primary thread");
		DSC_CONDITION_THROW(_multi_threaded_mode == true, "should not be run in multi threaded mode");
		return;
	}

private:
	const int _primary_thread_id;
	bool _multi_threaded_mode;
	std::map< std::string, std::shared_ptr< IDagThreadedNode > > _dag_node_map;

};

std::shared_ptr<DagThreadedCollection> DagThreadedCollection::Factory()
{
	return std::make_shared<DagThreadedCollection>();
}

DagThreadedCollection::DagThreadedCollection()
{
	_implementation = std::make_unique<DagThreadedCollectionImplementation>();
	return;
}

DagThreadedCollection::~DagThreadedCollection()
{
	try
	{
		_implementation->SetMultiThreadedMode(false);
		_implementation->CleanUp();
	}
	catch(...)
	{
		DSC_ASSERT_ALWAYS("clean up failed");
	}
	return;
}

void DagThreadedCollection::SetMultiThreadedMode(const bool in_enabled)
{
	_implementation->SetMultiThreadedMode(in_enabled);
	return;
}

DagThreadedCollection::NodeID DagThreadedCollection::FindNode(const std::string& in_name)
{
	return _implementation->FindNode(in_name);
}

DagThreadedCollection::NodeID DagThreadedCollection::CreateNodeVariable(
	const std::string& in_name, 
	const std::shared_ptr< IDagThreadedValue >& in_dag_value,
	const DagThreaded::DirtyCase in_dirty_case
	)
{
	return _implementation->CreateNodeVariable(in_name, in_dag_value, in_dirty_case);
}

DagThreadedCollection::NodeID DagThreadedCollection::CreateNodeCalculate(const std::string& in_name, const CalculateFunction& in_function)
{
	//return _implementation->CreateNodeCalculate(*this, in_name, in_function);
	return _implementation->CreateNodeCalculate(in_name, in_function);
}

void DagThreadedCollection::DestroyNode(const NodeID in_node_id)
{
	_implementation->DestroyNode(in_node_id);
	return;
}

void DagThreadedCollection::AddNodeLinkIndexed(const NodeID in_node_id_subject, const NodeID in_node_id_to_add, const int in_index)
{
	_implementation->AddNodeLinkIndexed(in_node_id_subject, in_node_id_to_add, in_index);
	return;
}

void DagThreadedCollection::RemoveNodeLinkIndexed(const NodeID in_node_id_subject, const int in_index)
{
	_implementation->RemoveNodeLinkIndexed(in_node_id_subject, in_index);
	return;
}

void DagThreadedCollection::AddNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_add)
{
	_implementation->AddNodeLinkStack(in_node_id_subject, in_node_id_to_add);
	return;
}

void DagThreadedCollection::RemoveNodeLinkStack(const NodeID in_node_id_subject, const NodeID in_node_id_to_remove)
{
	_implementation->RemoveNodeLinkStack(in_node_id_subject, in_node_id_to_remove);
	return;
}

void DagThreadedCollection::UnlinkNode(const NodeID in_node_id)
{
	_implementation->UnlinkNode(in_node_id);
	return;
}

std::shared_ptr< IDagThreadedValue > DagThreadedCollection::GetDagValue(const NodeID in_node_id)
{
	return _implementation->GetDagValue(in_node_id);
}

void DagThreadedCollection::SetDagValue(const NodeID in_node_id, const std::shared_ptr< IDagThreadedValue >& in_dag_value)
{
	return _implementation->SetDagValue(in_node_id, in_dag_value);
}

void DagThreadedCollection::CheckPrimaryThreadOrMultiThreadedMode() const
{
	return _implementation->CheckPrimaryThreadOrMultiThreadedMode();
}
