#pragma once

#include "common/dag_threaded/dag_threaded_value.h"

namespace DagThreadedHelper
{

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
