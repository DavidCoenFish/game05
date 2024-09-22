#pragma once

#include "common/dag/dag.h"
#include "common/dag/i_dag_value.h"

template< typename TYPE >
class DagValue : public IDagValue
{
public:
	DagValue(const TYPE& in_default) : _value(in_default)
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
		return Dag::IsZero<TYPE>(_value);
	}

	const bool Comparison(const IDagValue* const in_rhs) override
	{
		auto rhs = in_rhs ? dynamic_cast<const DagValue<TYPE>*>(in_rhs) : nullptr;
		if (nullptr != rhs)
		{
			return _value == rhs->_value;
		}
		return false;
	}

private:
	TYPE _value;

};
