#include "pch.h"

#include "DAG2/Dag2NodeCalculate.h"

Dag2NodeCalculate::Dag2NodeCalculate(const CalculateFunction& pCalculateCallback)
	: m_pCalculateCallback(pCalculateCallback)
{
	return;
}

Dag2NodeCalculate::~Dag2NodeCalculate()
{
	return;
}

void Dag2NodeCalculate::OnCalculate(
	std::shared_ptr< iDag2Value >& pValue,
	const std::vector< iDag2Value* >& arrayInputStack, 
	const std::vector< iDag2Value* >& arrayInputIndex
	)
{
	if (m_pCalculateCallback)
	{
		m_pCalculateCallback(pValue, arrayInputStack, arrayInputIndex);
	}
}
