#pragma once

#include "DAG2/iDag2Node.h"

class iDag2Value;

class Dag2NodeCalculateBase : public iDag2Node
{
public:
	Dag2NodeCalculateBase();
	~Dag2NodeCalculateBase();

	const bool GetDirty() const { return m_dirty; }

private:
	virtual void SetOutput(iDag2Node* const pNode) override;
	virtual void RemoveOutput(iDag2Node* const pNode) override;

	virtual void AddInputStack(iDag2Node* const pNode) override;
	virtual void RemoveInputStack(iDag2Node* const pNode) override;
	virtual void SetInputIndex(iDag2Node* const pNodeOrNullptr, const int index) override;

	virtual iDag2Value* GetValue() override;

	virtual void MarkDirty() override;
	virtual void Unlink() override;

	virtual void OnCalculate(
		std::shared_ptr< iDag2Value >& pValue,
		const std::vector< iDag2Value* >& arrayInputStack, 
		const std::vector< iDag2Value* >& arrayInputIndex
		) = 0;

private:
	std::vector< iDag2Node* > m_arrayInputStack;
	std::vector< iDag2Node* > m_arrayInputIndex;
	std::vector< iDag2Node* > m_arrayOutput;
	std::shared_ptr< iDag2Value > m_pValue;
	bool m_dirty;

};
