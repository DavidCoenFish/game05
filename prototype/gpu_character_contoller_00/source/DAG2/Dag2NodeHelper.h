#pragma once

#include "DAG2/Dag2NodeVariable.h"

class iDag2Node;

namespace Dag2NodeHelper
{
	template <typename TYPE>
	static void SetNodeValue(std::shared_ptr<iDag2Node>& pDag2Node, const TYPE& value)
	{
		auto pNodeVariable = std::dynamic_pointer_cast<Dag2NodeVariable>(pDag2Node);
		if (nullptr != pNodeVariable)
		{
			pNodeVariable->Assign(value);
		}
		else
		{
			LOG_MESSAGE_WARNING("Attempt at bad SetNodeValue");
		}
	}

	//no action if you try to assign to a calculate node
	template <typename TYPE>
	static void SetNodeValue(iDag2Node* pDag2Node, const TYPE& value)
	{
		auto pNodeVariable = dynamic_cast<Dag2NodeVariable*>(pDag2Node);
		if (nullptr != pNodeVariable)
		{
			pNodeVariable->Assign(value);
		}
		else
		{
			LOG_MESSAGE_WARNING("Attempt at bad SetNodeValue");
		}
	}

	template <typename TYPE>
	static const TYPE GetNodeValue(iDag2Node* pDag2Node, const TYPE defaultValue = 0)
	{
		iDag2Value* pDag2Value = pDag2Node ? pDag2Node->GetValue() : nullptr;
		auto pResultType = pDag2Value ? dynamic_cast<Dag2Value<TYPE>*>(pDag2Value) : nullptr;
		if (nullptr != pResultType)
		{
			return pResultType->Get();
		}
		LOG_MESSAGE_WARNING("Attempt to get value from Dag2Node %d %d", pDag2Value ? pDag2Value->GetTypeID() : -1, Dag2Value<TYPE>::StaticGetTypeID());
		return defaultValue;
	}

	template <typename TYPE>
	static const TYPE& GetNodeValueRef(iDag2Node* pDag2Node)
	{
		iDag2Value* pDag2Value = pDag2Node ? pDag2Node->GetValue() : nullptr;
		auto pResultType = pDag2Value ? dynamic_cast<Dag2Value<TYPE>*>(pDag2Value) : nullptr;
		if (nullptr != pResultType)
		{
			return pResultType->GetRef();
		}
		LOG_MESSAGE_WARNING("Attempt to get value ref from Dag2Node %d %d", pDag2Value ? pDag2Value->GetTypeID() : -1, Dag2Value<TYPE>::StaticGetTypeID());
		static const TYPE s_defaultValue;
		return s_defaultValue;
	}

};
