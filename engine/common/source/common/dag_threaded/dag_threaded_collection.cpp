#include "common/common_pch.h"
#include "common/dag_threaded/dag_threaded_collection.h"

#include "common/dag_threaded/dag_threaded_node_calculate.h"
#include "common/dag_threaded/dag_threaded_node_variable.h"
#include "common/dag_threaded/i_dag_threaded_node.h"
#include "common/platform.h"

class DagThreadedCollectionImplementation
{
public:
	DagThreadedCollectionImplementation()
	{
		//nop
	}
	~DagThreadedCollectionImplementation()
	{
		//nop
	}

	DagThreadedCollection::NodeID FindNode(const std::string& in_name)
	{
        std::shared_lock read_lock(_dag_node_map_mutex);
		auto found = _dag_node_map.find(in_name);
		if (found != _dag_node_map.end())
		{
			return found->second;
		}
		return nullptr;
	}

	DagThreadedCollection::NodeID CreateNodeVariable(
		const std::string& in_name, 
		const std::shared_ptr< IDagThreadedValue >& in_dag_value,
		const DagThreaded::DirtyCase in_dirty_case
		)
	{
		auto node_variable = std::make_shared<DagThreadedNodeVariable>(in_name, in_dag_value, in_dirty_case);
        {
            std::unique_lock write_lock(_dag_node_map_mutex);
            DSC_CONDITION_THROW(_dag_node_map.find(in_name) != _dag_node_map.end(), "CreateNodeVariable replacing existing node");
            _dag_node_map[in_name] = node_variable;
        }

		return node_variable;
	}

	DagThreadedCollection::NodeID CreateNodeCalculate(const std::string& in_name, const DagThreadedCollection::CalculateFunction& in_function)
	{
		auto node_calculate = std::make_shared<DagThreadedNodeCalculate>(in_name, in_function);
        {
            std::unique_lock write_lock(_dag_node_map_mutex);
            DSC_CONDITION_THROW(_dag_node_map.find(in_name) != _dag_node_map.end(), "CreateNodeCalculate replacing existing node");
            _dag_node_map[in_name] = node_calculate;
        }

		return node_calculate;
	}

	void DestroyNode(const DagThreadedCollection::NodeID& in_node_id)
	{
		if (nullptr != in_node_id)
		{
            std::unique_lock write_lock(_dag_node_map_mutex);
            auto found = _dag_node_map.find(in_node_id->GetName());
			if (found != _dag_node_map.end())
			{
				_dag_node_map.erase(found);
			}
		}
		return;
	}

	void AddNodeLinkIndexed(const DagThreadedCollection::NodeID& in_node_id_subject, const DagThreadedCollection::NodeID& in_node_id_to_add, const int in_index)
	{
		auto node_subject = std::dynamic_pointer_cast<DagThreadedNodeCalculate>(in_node_id_subject);
		if (nullptr != node_subject)
		{
			node_subject->SetInputIndex(in_node_id_to_add.get(), in_index);
		}
		return;
	}
	void RemoveNodeLinkIndexed(const DagThreadedCollection::NodeID in_node_id_subject, const int in_index)
	{
        auto node_subject = std::dynamic_pointer_cast<DagThreadedNodeCalculate>(in_node_id_subject);
        if (nullptr != node_subject)
		{
			node_subject->SetInputIndex(nullptr, in_index);
		}
		return;
	}
	void AddNodeLinkStack(const DagThreadedCollection::NodeID& in_node_id_subject, const DagThreadedCollection::NodeID& in_node_id_to_add)
	{
        auto node_subject = std::dynamic_pointer_cast<DagThreadedNodeCalculate>(in_node_id_subject);
		if (nullptr != node_subject)
		{
			node_subject->AddInputStack(in_node_id_to_add.get());
		}
		return;
	}
	void RemoveNodeLinkStack(const DagThreadedCollection::NodeID& in_node_id_subject, const DagThreadedCollection::NodeID& in_node_id_to_remove)
	{
        auto node_subject = std::dynamic_pointer_cast<DagThreadedNodeCalculate>(in_node_id_subject);
        if (nullptr != node_subject)
		{
			node_subject->RemoveInputStack(in_node_id_to_remove.get());
		}
	}
	void UnlinkNode(const DagThreadedCollection::NodeID& in_node_id)
	{
        auto node_calculate = std::dynamic_pointer_cast<DagThreadedNodeCalculate>(in_node_id);
		if (nullptr != node_calculate)
		{
			node_calculate->Unlink();
		}
		return;
	}

	std::shared_ptr< IDagThreadedValue > GetDagValue(const DagThreadedCollection::NodeID& in_node_id)
	{
		if (nullptr != in_node_id)
		{
			return in_node_id->GetValue();
		}
		return nullptr;
	}

	void SetDagValue(const DagThreadedCollection::NodeID& in_node_id, const std::shared_ptr< IDagThreadedValue >& in_dag_value)
	{
        auto node_variable = std::dynamic_pointer_cast<DagThreadedNodeVariable>(in_node_id);
		if (nullptr != node_variable)
		{
			node_variable->SetValue(in_dag_value);
		}
		return;
	}

private:
    std::shared_mutex _dag_node_map_mutex;
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
    _implementation.reset();
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
	return _implementation->CreateNodeCalculate(in_name, in_function);
}

void DagThreadedCollection::DestroyNode(const NodeID& in_node_id)
{
	_implementation->DestroyNode(in_node_id);
	return;
}

void DagThreadedCollection::AddNodeLinkIndexed(const NodeID& in_node_id_subject, const NodeID& in_node_id_to_add, const int in_index)
{
	_implementation->AddNodeLinkIndexed(in_node_id_subject, in_node_id_to_add, in_index);
	return;
}

void DagThreadedCollection::RemoveNodeLinkIndexed(const NodeID& in_node_id_subject, const int in_index)
{
	_implementation->RemoveNodeLinkIndexed(in_node_id_subject, in_index);
	return;
}

void DagThreadedCollection::AddNodeLinkStack(const NodeID& in_node_id_subject, const NodeID& in_node_id_to_add)
{
	_implementation->AddNodeLinkStack(in_node_id_subject, in_node_id_to_add);
	return;
}

void DagThreadedCollection::RemoveNodeLinkStack(const NodeID& in_node_id_subject, const NodeID& in_node_id_to_remove)
{
	_implementation->RemoveNodeLinkStack(in_node_id_subject, in_node_id_to_remove);
	return;
}

void DagThreadedCollection::UnlinkNode(const NodeID& in_node_id)
{
	_implementation->UnlinkNode(in_node_id);
	return;
}

std::shared_ptr< IDagThreadedValue > DagThreadedCollection::GetDagValue(const NodeID& in_node_id)
{
	return _implementation->GetDagValue(in_node_id);
}

void DagThreadedCollection::SetDagValue(const NodeID& in_node_id, const std::shared_ptr< IDagThreadedValue >& in_dag_value)
{
	return _implementation->SetDagValue(in_node_id, in_dag_value);
}
