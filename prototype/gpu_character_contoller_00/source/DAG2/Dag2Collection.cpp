#include "pch.h"

#include "DAG2/Dag2Collection.h"
#include "DAG2/iDag2Node.h"

Dag2Collection::Dag2Collection()
{
	return;
}

Dag2Collection::~Dag2Collection()
{
	for(auto iter: m_dagNodeMap)
	{
		iter.second->Unlink();
	}
	return;
}

const std::string Dag2Collection::MakeUniqueName()
{
	static int s_count = 0;
	const std::string result = std::string("__defaultName") + std::to_string(s_count++);
	return result;
}

void Dag2Collection::AddNode(const std::string& name, const std::shared_ptr< iDag2Node >& in_dagNode)
{
	auto found = m_dagNodeMap.find(name);
	if (found != m_dagNodeMap.end())
	{
		assert(nullptr == found->second);
	}
	m_dagNodeMap[name] = in_dagNode;
}

void Dag2Collection::RemoveNode(const std::string& name)
{
	auto found = m_dagNodeMap.find(name);
	if (found != m_dagNodeMap.end())
	{
		found->second->Unlink();
		m_dagNodeMap.erase(found);
	}
	return;
}

std::shared_ptr< iDag2Node > Dag2Collection::GetNode(const std::string& name) const
{
	auto found = m_dagNodeMap.find(name);
	if (found != m_dagNodeMap.end())
	{
		return found->second;
	}
	return nullptr;
}

void Dag2Collection::AddLinkStack(const std::shared_ptr< iDag2Node >& pInput, const std::shared_ptr< iDag2Node >& pOutput)
{
	if (nullptr != pOutput)
	{
		pOutput->AddInputStack(pInput.get());
		pOutput->MarkDirty();
	}
	return;
}

void Dag2Collection::RemoveLinkStack(const std::shared_ptr< iDag2Node >& pInput, const std::shared_ptr< iDag2Node >& pOutput)
{
	if (nullptr != pOutput)
	{
		pOutput->RemoveInputStack(pInput.get());
		pOutput->MarkDirty();
	}
	return;
}

void Dag2Collection::SetLinkIndex(const std::shared_ptr< iDag2Node >& pInputOrNullptr, const int inputIndex, const std::shared_ptr< iDag2Node >& pOutput)
{
	if (nullptr != pOutput)
	{
		pOutput->SetInputIndex(pInputOrNullptr.get(), inputIndex);
		pOutput->MarkDirty();
	}
	return;
}
