#pragma once

#include "DAG2/iDag2Node.h"
#include "DAG2/Dag2Value.h"
#include "DAG2/Dag2ValueHelper.h"
#include "DAG2/Dag2.h"

class iDag2Value;

class Dag2NodeVariable : public iDag2Node
{
public:
	template< typename TYPE >
	static std::shared_ptr< Dag2NodeVariable > Factory(
		const TYPE& value,
		const Dag2::DirtyCase dirtyCase = Dag2::DirtyCase::None
		)
	{
		auto pValue = std::make_shared< Dag2Value<TYPE> >(value);
		auto pResult = std::make_shared< Dag2NodeVariable >(
			pValue,
			dirtyCase
			);

		return pResult;
	}

	explicit Dag2NodeVariable(
		const std::shared_ptr< iDag2Value >& pValue = nullptr,
		const Dag2::DirtyCase dirtyCase = Dag2::DirtyCase::None
		);

	template <typename TYPE>
	void Assign(const TYPE& newValue)
	{
		const bool bDirty = Dag2ValueHelper::Assign(m_pValue, newValue, m_dirtyCase);
		if (true == bDirty)
		{
			MarkDirty();
		}
		return;
	}

private:
	virtual void SetOutput(iDag2Node* const pNode) override;
	virtual void RemoveOutput(iDag2Node* const pNode) override;

	virtual void AddInputStack(iDag2Node* const pNode) override;
	virtual void RemoveInputStack(iDag2Node* const pNode) override;
	virtual void SetInputIndex(iDag2Node* const pNode, const int index) override;

	virtual iDag2Value* GetValue() override;

	virtual void MarkDirty() override;
	virtual void Unlink() override;

private:
	std::vector< iDag2Node* > m_arrayOutput;
	std::shared_ptr< iDag2Value > m_pValue;
	Dag2::DirtyCase m_dirtyCase;

};

