#pragma once

#include "common/dag_threaded/dag_threaded.h"
#include "common/dag_threaded/i_dag_threaded_value.h"

template< typename TYPE >
class DagThreadedValue : public IDagThreadedValue
{
public:
	DagThreadedValue(const TYPE& in_default) : _value(in_default)
	{
		return;
	}

	const TYPE Get() const
	{
		return _value;
	}

	const TYPE& GetRef() const
	{
		return _value;
	}

private:
	const bool IsZero() const override
	{
		return DagThreaded::IsZero<TYPE>(_value);
	}

	const bool Comparison(const IDagThreadedValue* const in_rhs) override
	{
		auto rhs = in_rhs ? dynamic_cast<const DagThreadedValue<TYPE>*>(in_rhs) : nullptr;
		if (nullptr != rhs)
		{
			return _value == rhs->_value;
		}
		return false;
	}

private:
	TYPE _value;

};
