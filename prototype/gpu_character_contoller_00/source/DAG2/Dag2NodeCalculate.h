#pragma once

#include "DAG2/Dag2NodeCalculateBase.h"

class iDag2Value;

class Dag2NodeCalculate : public Dag2NodeCalculateBase
{
public:
	//array stack, array indexed, value
	typedef std::function< void (
		std::shared_ptr< iDag2Value >&,
		const std::vector< iDag2Value* >&, 
		const std::vector< iDag2Value* >&
		) > CalculateFunction;

	static std::shared_ptr< Dag2NodeCalculate > Factory(
		const CalculateFunction& pCalculateCallback
		)
	{
		auto pResult = std::make_shared< Dag2NodeCalculate >(
			pCalculateCallback
			);

		return pResult;
	}

	Dag2NodeCalculate(const CalculateFunction& pCalculateCallback);
	~Dag2NodeCalculate();

private:
	virtual void OnCalculate(
		std::shared_ptr< iDag2Value >& pValue,
		const std::vector< iDag2Value* >& arrayInputStack, 
		const std::vector< iDag2Value* >& arrayInputIndex
		) override;

private:
	CalculateFunction m_pCalculateCallback;

};
