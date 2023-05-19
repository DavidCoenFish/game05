#pragma once

//#include "DAG2/iDag2Value.h"
#include "DAG2/Dag2.h"
#include "DAG2/Dag2Value.h"
#include "Log.h"

namespace Dag2ValueHelper
{
	template <typename TYPE>
	const bool Assign(
		std::shared_ptr< iDag2Value >& pValueHolder, 
		const TYPE& newValue,
		const Dag2::DirtyCase dirtyCase = Dag2::DirtyCase::None
		)
	{
		bool bValueChange = false;

		if ((nullptr == pValueHolder) ||
			(pValueHolder->GetTypeID() != Dag2Value<TYPE>::StaticGetTypeID()))
		{
			pValueHolder = std::make_shared<Dag2Value<TYPE>>(newValue);
			bValueChange = true;
		}
		else
		{
			bValueChange = pValueHolder->InternalAssign(&newValue);
		}
		
		bool bMarkOutputDirty = false;
		if (0 != (dirtyCase & Dag2::DirtyCase::ValueChanged))
		{
			bMarkOutputDirty = bValueChange;
		}
		if (nullptr != pValueHolder)
		{
			if ((false == bMarkOutputDirty) && 
				(0 != (dirtyCase & Dag2::DirtyCase::NewValueNotZero)))
			{
				bMarkOutputDirty = (false == pValueHolder->IsZero());
			}
		}

		return bMarkOutputDirty;
	}

	template <typename TYPE>
	const TYPE Get(iDag2Value* pDag2Value, const TYPE defaultValue = 0)
	{
		auto pResultType = dynamic_cast< Dag2Value<TYPE>* >(pDag2Value);
		if (nullptr != pResultType)
		{
			return pResultType->Get();
		}
		LOG_MESSAGE_WARNING("Attempt to get value from Dag2Value %d %d", pDag2Value ? pDag2Value->GetTypeID() : -1, Dag2Value<TYPE>::StaticGetTypeID());
		return defaultValue;
	}

	template <typename TYPE>
	const TYPE& GetRef(iDag2Value* pDag2Value)
	{
		auto pResultType = dynamic_cast< Dag2Value<TYPE>* >(pDag2Value);
		if (nullptr != pResultType)
		{
			return pResultType->GetRef();
		}
		LOG_MESSAGE_WARNING("Attempt to get ref value from Dag2Value %d %d", pDag2Value ? pDag2Value->GetTypeID() : -1, Dag2Value<TYPE>::StaticGetTypeID());
		static const TYPE s_defaultValue = TYPE();
		return s_defaultValue;
	}


};
