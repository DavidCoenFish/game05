#pragma once

#include "common/dag_threaded/dag_threaded.h"
#include "common/dag_threaded/i_dag_threaded_value.h"
//#include "common/util/id.h"

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
	
	//static const int StaticGetTypeID()
	//{
	//	return s_typeID;
	//}

private:
	//const int GetTypeID() const override
	//{
	//	return s_typeID;
	//}

	const bool IsZero() const override
	{
		return DagThreaded::template IsZero<TYPE>(_value);
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

	//const bool Comparison(const IDagThreadedValue* const in_rhs) override
	//{
	//	if (nullptr == in_rhs)
	//	{
	//		return false;
	//	}
	//	if (s_typeID != in_rhs->GetTypeID())
	//	{
	//		return false;
	//	}
	//	if (_value != ((DagThreadedValue<TYPE>*)in_rhs)->_value)
	//	{
	//		return false;
	//	}
	//	return true;
	//}

	//virtual const bool InternalAssign(const void* const in_new_value) override
	//{
	//	bool change = false;
	//	if (_value != (*(const TYPE* const )in_new_value))
	//	{
	//		change = true;
	//		_value = (*(const TYPE* const )in_new_value);
	//	}
	//	return change;
	//}

private:
	TYPE _value;

//public:
	//static int s_typeID;

};

//template< typename TYPE >
//int DagThreadedValue< TYPE >::s_typeID = ID::GetID();
