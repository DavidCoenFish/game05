#include "pch.h"

#include "DAG2/Dag2NodeVariable.h"
#include "DAG2/iDag2Value.h"
#include "DAG2/Dag2.h"

Dag2NodeVariable::Dag2NodeVariable(
	const std::shared_ptr< iDag2Value >& pValue,
	const Dag2::DirtyCase dirtyCase
	)
	: m_arrayOutput()
	, m_pValue(pValue)
	, m_dirtyCase(dirtyCase)
{
}

void Dag2NodeVariable::SetOutput(iDag2Node* const pNode)
{
	if (nullptr != pNode)
	{
		m_arrayOutput.push_back(pNode);
	}
}

void Dag2NodeVariable::RemoveOutput(iDag2Node* const pNode)
{
	m_arrayOutput.erase(std::remove(m_arrayOutput.begin(), m_arrayOutput.end(), pNode), m_arrayOutput.end());
}

void Dag2NodeVariable::AddInputStack(iDag2Node* const)
{
	assert( false ); //not applicable to nodes variable
	return;
}

void Dag2NodeVariable::RemoveInputStack(iDag2Node* const)
{
	assert( false ); //not applicable to nodes variable
	return;
}

void Dag2NodeVariable::SetInputIndex(iDag2Node* const, const int)
{
	assert( false ); //not applicable to nodes variable
	return;
}

iDag2Value* Dag2NodeVariable::GetValue()
{
	return m_pValue.get();
}

void Dag2NodeVariable::MarkDirty()
{
	for (auto pIter : m_arrayOutput )
	{
		pIter->MarkDirty();
	}
}

void Dag2NodeVariable::Unlink()
{
	//nop
}

