#include "pch.h"

#include "DAG2/Dag2NodeCalculateBase.h"
#include "DAG2/iDag2Node.h"
#include "DAG2/iDag2Value.h"

Dag2NodeCalculateBase::Dag2NodeCalculateBase()
	: m_dirty(false)
{
	return;
}

Dag2NodeCalculateBase::~Dag2NodeCalculateBase()
{
	//so, do we enforce safe teardown of dag collection?
	assert(0 == m_arrayOutput.size());
	return;
}

void Dag2NodeCalculateBase::SetOutput(iDag2Node* const pNode)
{
	if (nullptr != pNode)
	{
		m_arrayOutput.push_back(pNode);
	}
}

void Dag2NodeCalculateBase::RemoveOutput(iDag2Node* const pNode)
{
	m_arrayOutput.erase(std::remove(m_arrayOutput.begin(), m_arrayOutput.end(), pNode), m_arrayOutput.end());
}

void Dag2NodeCalculateBase::AddInputStack(iDag2Node* const pNode)
{
	if (nullptr != pNode)
	{
		m_arrayInputStack.push_back(pNode);
		pNode->SetOutput(this);
	}
	return;
}

void Dag2NodeCalculateBase::RemoveInputStack(iDag2Node* const pNode)
{
	if (nullptr == pNode)
	{
		return;
	}
	pNode->RemoveOutput(this);

	m_arrayInputStack.erase(std::remove(m_arrayInputStack.begin(), m_arrayInputStack.end(), pNode), m_arrayInputStack.end());

	return;
}

void Dag2NodeCalculateBase::SetInputIndex(iDag2Node* const pNodeOrNullptr, const int index)
{
	assert(0 <= index);
	if (m_arrayInputIndex.size() <= index)
	{
		m_arrayInputIndex.resize((size_t)index + 1);
	}
	{
		iDag2Node* const pBefore = m_arrayInputIndex[index];
		if (nullptr != pBefore)
		{
			pBefore->RemoveOutput(this);
		}
	}
	m_arrayInputIndex[index] = pNodeOrNullptr;
	if (nullptr != pNodeOrNullptr)
	{
		pNodeOrNullptr->SetOutput(this);
	}

	return;
}

iDag2Value* Dag2NodeCalculateBase::GetValue()
{
	if (true == m_dirty)
	{
		m_dirty = false;

		std::vector<iDag2Value*> arrayInputStack; 
		for(auto iter: m_arrayInputStack)
		{
			iDag2Value* pValue = iter ? iter->GetValue() : nullptr;
			arrayInputStack.push_back(pValue);
		}
		std::vector<iDag2Value*> arrayInputIndex; 
		for(auto iter: m_arrayInputIndex)
		{
			iDag2Value* pValue = iter ? iter->GetValue() : nullptr;
			arrayInputIndex.push_back(pValue);
		}

		OnCalculate(
			m_pValue, 
			arrayInputStack, 
			arrayInputIndex
			);
	}
	return m_pValue.get();
}

void Dag2NodeCalculateBase::MarkDirty()
{
	if (true != m_dirty)
	{
		m_dirty = true;
		for (auto pIter : m_arrayOutput)
		{
			pIter->MarkDirty();
		}
	}
}

void Dag2NodeCalculateBase::Unlink()
{
	for(auto iter: m_arrayInputStack)
	{
		if (nullptr != iter)
		{
			iter->RemoveOutput(this);
		}
	}
	m_arrayInputStack.clear();
	for(auto iter: m_arrayInputIndex)
	{
		if (nullptr != iter)
		{
			iter->RemoveOutput(this);
		}
	}
	m_arrayInputIndex.clear();
}

