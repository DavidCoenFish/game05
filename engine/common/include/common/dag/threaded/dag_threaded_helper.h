#pragma once

#include "common/dag/threaded/dag_threaded_value.h"

namespace DagThreadedHelper
{
	// Design: CreateDagValue should be able to be created on any thread
	template <typename TYPE>
	std::shared_ptr<IDagThreadedValue> CreateDagValue(const TYPE in_value)
	{
		return std::make_shared< DagThreadedValue< TYPE > >(in_value);
	}

	template <typename TYPE>
	const TYPE GetValue(const std::shared_ptr< IDagThreadedValue >& in_dag_value)
	{
		auto dag_value = dynamic_cast< DagThreadedValue< TYPE >* >(in_dag_value.get());
		if (nullptr != dag_value)
		{
			return dag_value->Get();
		}
		static TYPE s_default(0);
		return s_default;
	}

	template <typename TYPE>
	const TYPE& GetValueRef(const std::shared_ptr< IDagThreadedValue >& in_dag_value)
	{
		auto dag_value = dynamic_cast< DagThreadedValue< TYPE >* >(in_dag_value.get());
		if (nullptr != dag_value)
		{
			return dag_value->GetRef();
		}
		static TYPE s_default(0);
		return s_default;
	}
};
