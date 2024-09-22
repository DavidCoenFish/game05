#include "common/common_pch.h"
#include "common/dag/dag_collection.h"

#include "common/dag/dag_node_calculate.h"
#include "common/dag/dag_node_variable.h"
#include "common/dag/dag_value.h"
#include "common/dag/i_dag_node.h"
#include "common/dag/i_dag_value.h"


class DagCollectionImplementation
{
public:
	DagCollectionImplementation()
	{
		//nop
	}
	~DagCollectionImplementation()
	{
		//nop
	}

	DagCollection::NodeID FindNode(const std::string& in_name)
	{
		auto found = _dag_node_map.find(in_name);
		if (found != _dag_node_map.end())
		{
			return found->second;
		}
		return nullptr;
	}

	DagCollection::NodeID CreateNodeVariable(
		const std::string& in_name, 
		const std::shared_ptr< IDagValue >& in_dag_value,
		const Dag::DirtyCase in_dirty_case
		)
	{
		auto node_variable = std::make_shared<DagNodeVariable>(in_name, in_dag_value, in_dirty_case);
        {
            DSC_CONDITION_THROW(_dag_node_map.find(in_name) != _dag_node_map.end(), "CreateNodeVariable replacing existing node");
            _dag_node_map[in_name] = node_variable;
        }

		return node_variable;
	}

	DagCollection::NodeID CreateNodeCalculate(const std::string& in_name, const DagCollection::CalculateFunction& in_function)
	{
		auto node_calculate = std::make_shared<DagNodeCalculate>(in_name, in_function);
        {
            DSC_CONDITION_THROW(_dag_node_map.find(in_name) != _dag_node_map.end(), "CreateNodeCalculate replacing existing node");
            _dag_node_map[in_name] = node_calculate;
        }

		return node_calculate;
	}

	void DestroyNode(const DagCollection::NodeID& in_node_id)
	{
		if (nullptr != in_node_id)
		{
            auto found = _dag_node_map.find(in_node_id->GetName());
			if (found != _dag_node_map.end())
			{
				_dag_node_map.erase(found);
			}
		}
		return;
	}

	void AddNodeLinkIndexed(const DagCollection::NodeID& in_node_id_subject, const DagCollection::NodeID& in_node_id_to_add, const int in_index)
	{
		auto node_subject = std::dynamic_pointer_cast<DagNodeCalculate>(in_node_id_subject);
		if (nullptr != node_subject)
		{
			node_subject->SetInputIndex(in_node_id_to_add.get(), in_index);
		}
		return;
	}
	void RemoveNodeLinkIndexed(const DagCollection::NodeID in_node_id_subject, const int in_index)
	{
        auto node_subject = std::dynamic_pointer_cast<DagNodeCalculate>(in_node_id_subject);
        if (nullptr != node_subject)
		{
			node_subject->SetInputIndex(nullptr, in_index);
		}
		return;
	}
	void AddNodeLinkStack(const DagCollection::NodeID& in_node_id_subject, const DagCollection::NodeID& in_node_id_to_add)
	{
        auto node_subject = std::dynamic_pointer_cast<DagNodeCalculate>(in_node_id_subject);
		if (nullptr != node_subject)
		{
			node_subject->AddInputStack(in_node_id_to_add.get());
		}
		return;
	}
	void RemoveNodeLinkStack(const DagCollection::NodeID& in_node_id_subject, const DagCollection::NodeID& in_node_id_to_remove)
	{
        auto node_subject = std::dynamic_pointer_cast<DagNodeCalculate>(in_node_id_subject);
        if (nullptr != node_subject)
		{
			node_subject->RemoveInputStack(in_node_id_to_remove.get());
		}
	}
	void UnlinkNode(const DagCollection::NodeID& in_node_id)
	{
        auto node_calculate = std::dynamic_pointer_cast<DagNodeCalculate>(in_node_id);
		if (nullptr != node_calculate)
		{
			node_calculate->Unlink();
		}
		return;
	}

	std::shared_ptr< IDagValue > GetDagValue(const DagCollection::NodeID& in_node_id)
	{
		if (nullptr != in_node_id)
		{
			return in_node_id->GetValue();
		}
		return nullptr;
	}

	void SetDagValue(const DagCollection::NodeID& in_node_id, const std::shared_ptr< IDagValue >& in_dag_value)
	{
        auto node_variable = std::dynamic_pointer_cast<DagNodeVariable>(in_node_id);
		if (nullptr != node_variable)
		{
			node_variable->SetValue(in_dag_value);
		}
		return;
	}

    std::shared_ptr<Tooltip> DagCollection(const DagCollection::NodeID& in_node_id)
    {
    }

private:
	std::map< std::string, std::shared_ptr< IDagNode > > _dag_node_map;

};

std::shared_ptr<DagCollection> DagCollection::Factory()
{
    return std::make_shared<DagCollection>();
}

DagCollection::DagCollection()
{
	_implementation = std::make_unique<DagCollectionImplementation>();
	return;
}

DagCollection::~DagCollection()
{
    _implementation.reset();
    return;
}

DagCollection::NodeID DagCollection::FindNode(const std::string& in_name)
{
	return _implementation->FindNode(in_name);
}

DagCollection::NodeID DagCollection::CreateNodeVariable(
	const std::string& in_name, 
	const std::shared_ptr< IDagValue >& in_dag_value = nullptr,
	const Dag::DirtyCase in_dirty_case = Dag::DirtyCase::ValueChanged
	)
{
    return _implementation->CreateNodeVariable(in_name, in_dag_value, in_dirty_case);
}

DagCollection::NodeID DagCollection::CreateNodeCalculate(const std::string& in_name, const CalculateFunction& in_function)
{
	return _implementation->CreateNodeCalculate(in_name, in_function);
}

void DagCollection::DestroyNode(const NodeID& in_node_id)
{
	_implementation->DestroyNode(in_node_id);
	return;
}

void DagCollection::AddNodeLinkIndexed(const NodeID& in_node_id_subject, const NodeID& in_node_id_to_add, const int in_index)
{
	_implementation->AddNodeLinkIndexed(in_node_id_subject, in_node_id_to_add, in_index);
	return;
}

void DagCollection::RemoveNodeLinkIndexed(const NodeID& in_node_id_subject, const int in_index)
{
	_implementation->RemoveNodeLinkIndexed(in_node_id_subject, in_index);
	return;
}

void DagCollection::AddNodeLinkStack(const NodeID& in_node_id_subject, const NodeID& in_node_id_to_add)
{
	_implementation->AddNodeLinkStack(in_node_id_subject, in_node_id_to_add);
	return;
}

void DagCollection::RemoveNodeLinkStack(const NodeID& in_node_id_subject, const NodeID& in_node_id_to_remove)
{
	_implementation->RemoveNodeLinkStack(in_node_id_subject, in_node_id_to_remove);
	return;
}

void DagCollection::UnlinkNode(const NodeID& in_node_id)
{
	_implementation->UnlinkNode(in_node_id);
	return;
}

std::shared_ptr< IDagValue > DagCollection::GetDagValue(const NodeID& in_node_id)
{
	return _implementation->GetDagValue(in_node_id);
}

void DagCollection::SetDagValue(const NodeID& in_node_id, const std::shared_ptr< IDagValue >& in_dag_value)
{
	return _implementation->SetDagValue(in_node_id, in_dag_value);
}

std::shared_ptr<Tooltip> DagCollection::MakeTooltip(const NodeID& in_node_id)
{
	return _implementation->MakeTooltip(in_node_id);
}

