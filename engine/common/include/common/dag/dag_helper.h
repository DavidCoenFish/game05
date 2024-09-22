#pragma once

#pragma once

#include "common/dag/dag_value.h"

namespace DagHelper
{
	// Design: CreateDagValue should be able to be created on any thread
	template <typename TYPE>
	std::shared_ptr<IDagValue> CreateDagValue(const TYPE in_value)
	{
		return std::make_shared< DagValue< TYPE > >(in_value);
	}

	template <typename TYPE>
	const TYPE GetValue(const std::shared_ptr< IDagValue >& in_dag_value)
	{
		auto dag_value = dynamic_cast< DagValue< TYPE >* >(in_dag_value.get());
		if (nullptr != dag_value)
		{
			return dag_value->Get();
		}
		static TYPE s_default(0);
		return s_default;
	}

	template <typename TYPE>
	const TYPE& GetValueRef(const std::shared_ptr< IDagValue >& in_dag_value)
	{
		auto dag_value = dynamic_cast< DagValue< TYPE >* >(in_dag_value.get());
		if (nullptr != dag_value)
		{
			return dag_value->GetRef();
		}
		static TYPE s_default(0);
		return s_default;
	}
};
