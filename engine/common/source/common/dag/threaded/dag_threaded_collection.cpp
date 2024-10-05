#include "common/common_pch.h"
#include "common/dag/threaded/dag_threaded_collection.h"

#include "common/dag/threaded/dag_threaded_node_calculate.h"
#include "common/dag/threaded/dag_threaded_node_variable.h"
#include "common/dag/threaded/i_dag_threaded_node.h"
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

    const std::string MakeUid()
    {
        std::string result;
        do 
        {
            const int trace = _auto_generate_uid.fetch_add(1);
            result = std::string("_auto_uid_") + std::to_string(trace);
        }
        while(_dag_node_map.find(result) != _dag_node_map.end());
        return result;
    }

	DagThreadedCollection::NodeID FindNode(const std::string& in_name)
	{
        std::shared_lock read_lock(_dag_node_map_mutex);
		auto found = _dag_node_map.find(in_name);
		if (found != _dag_node_map.end())
		{
			return found->second.get();
		}
		return nullptr;
	}

	DagThreadedCollection::NodeID CreateNodeVariable(
		const std::string& in_uid, 
		const std::shared_ptr< IDagThreadedValue >& in_dag_value,
		const DagThreaded::DirtyCase in_dirty_case,
        const std::string& in_display_name
		)
	{
		auto node_variable = std::make_shared<DagThreadedNodeVariable>(in_uid, in_dag_value, in_dirty_case, in_display_name);
        {
            std::unique_lock write_lock(_dag_node_map_mutex);
            DSC_CONDITION_THROW(_dag_node_map.find(in_uid) != _dag_node_map.end(), "CreateNodeVariable replacing existing node");
            _dag_node_map[in_uid] = node_variable;
        }

		return node_variable.get();
	}

	DagThreadedCollection::NodeID CreateNodeCalculate(
        const std::string& in_uid, 
        const DagThreadedCollection::CalculateFunction& in_function,
        const std::string& in_display_name,
        const std::string& in_tooltip_body
        )
	{
		auto node_calculate = std::make_shared<DagThreadedNodeCalculate>(in_uid, in_function, in_display_name, in_tooltip_body);
        {
            std::unique_lock write_lock(_dag_node_map_mutex);
            DSC_CONDITION_THROW(_dag_node_map.find(in_uid) != _dag_node_map.end(), "CreateNodeCalculate replacing existing node");
            _dag_node_map[in_uid] = node_calculate;
        }

		return node_calculate.get();
	}

	void DestroyNode(const DagThreadedCollection::NodeID in_node_id)
	{
		if (nullptr != in_node_id)
		{
            std::unique_lock write_lock(_dag_node_map_mutex);
            auto found = _dag_node_map.find(in_node_id->GetUid());
			if (found != _dag_node_map.end())
			{
				_dag_node_map.erase(found);
			}
		}
		return;
	}

	void AddNodeLinkIndexed(const DagThreadedCollection::NodeID in_node_id_subject, const DagThreadedCollection::NodeID in_node_id_to_add, const int in_index)
	{
        auto node_subject = dynamic_cast<DagThreadedNodeCalculate*>(in_node_id_subject);

		if (nullptr != node_subject)
		{
			node_subject->SetInputIndex(in_node_id_to_add, in_index);
		}
		return;
	}
	void RemoveNodeLinkIndexed(const DagThreadedCollection::NodeID in_node_id_subject, const int in_index)
	{
        auto node_subject = dynamic_cast<DagThreadedNodeCalculate*>(in_node_id_subject);
        if (nullptr != node_subject)
		{
			node_subject->SetInputIndex(nullptr, in_index);
		}
		return;
	}
	void AddNodeLinkStack(const DagThreadedCollection::NodeID in_node_id_subject, const DagThreadedCollection::NodeID in_node_id_to_add)
	{
        auto node_subject = dynamic_cast<DagThreadedNodeCalculate*>(in_node_id_subject);
		if (nullptr != node_subject)
		{
			node_subject->AddInputStack(in_node_id_to_add);
		}
		return;
	}
	void RemoveNodeLinkStack(const DagThreadedCollection::NodeID in_node_id_subject, const DagThreadedCollection::NodeID in_node_id_to_remove)
	{
        auto node_subject = dynamic_cast<DagThreadedNodeCalculate*>(in_node_id_subject);
        if (nullptr != node_subject)
		{
			node_subject->RemoveInputStack(in_node_id_to_remove);
		}
	}
	void UnlinkNode(const DagThreadedCollection::NodeID in_node_id)
	{
        auto node_calculate = dynamic_cast<DagThreadedNodeCalculate*>(in_node_id);
		if (nullptr != node_calculate)
		{
			node_calculate->Unlink();
		}
		return;
	}

	std::shared_ptr< IDagThreadedValue > GetDagValue(const DagThreadedCollection::NodeID in_node_id)
	{
		if (nullptr != in_node_id)
		{
			return in_node_id->GetValue();
		}
		return nullptr;
	}

	void SetDagValue(const DagThreadedCollection::NodeID in_node_id, const std::shared_ptr< IDagThreadedValue >& in_dag_value)
	{
        auto node_variable = dynamic_cast<DagThreadedNodeVariable*>(in_node_id);
		if (nullptr != node_variable)
		{
			node_variable->SetValue(in_dag_value);
		}
		return;
	}

private:
    std::shared_mutex _dag_node_map_mutex;
	std::map< std::string, std::shared_ptr< IDagThreadedNode > > _dag_node_map;

    std::atomic<int> _auto_generate_uid = 0;

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

const std::string DagThreadedCollection::MakeUid()
{
	return _implementation->MakeUid();
}

DagThreadedCollection::NodeID DagThreadedCollection::FindNode(const std::string& in_name)
{
	return _implementation->FindNode(in_name);
}

DagThreadedCollection::NodeID DagThreadedCollection::CreateNodeVariable(
	const std::string& in_uid, 
	const std::shared_ptr< IDagThreadedValue >& in_dag_value,
	const DagThreaded::DirtyCase in_dirty_case,
    const std::string& in_display_name
	)
{
	return _implementation->CreateNodeVariable(in_uid, in_dag_value, in_dirty_case, in_display_name);
}

DagThreadedCollection::NodeID DagThreadedCollection::CreateNodeCalculate(
    const std::string& in_uid, 
    const CalculateFunction& in_function,    
    const std::string& in_display_name,
    const std::string& in_tooltip_raw
)
{
	return _implementation->CreateNodeCalculate(in_uid, in_function, in_display_name, in_tooltip_raw);
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

const bool DagThreadedCollection::VisitNode(const NodeID in_node_id, IDagThreadedVisitor& in_visitor)
{
	return in_node_id->Visit(in_visitor);
}

const std::string DagThreadedCollection::GetDisplayName(const NodeID in_node_id)
{
    return in_node_id->GetDisplayName();
}

const std::string DagThreadedCollection::GetTooltipRaw(const NodeID in_node_id)
{
    return in_node_id->GetTooltipRaw();
}

std::shared_ptr< IDagThreadedValue > DagThreadedCollection::GetDagValue(const NodeID in_node_id)
{
	return _implementation->GetDagValue(in_node_id);
}

void DagThreadedCollection::SetDagValue(const NodeID in_node_id, const std::shared_ptr< IDagThreadedValue >& in_dag_value)
{
	return _implementation->SetDagValue(in_node_id, in_dag_value);
}
