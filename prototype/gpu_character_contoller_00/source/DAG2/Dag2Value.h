#pragma once

#include "DAG2/Dag2.h"
#include "DAG2/iDag2Value.h"
#include "ID.h"

template< typename TYPE >
class Dag2Value : public iDag2Value
{
public:
	Dag2Value(const TYPE& in_default) : m_value(in_default)
	{
		return;
	}

	const TYPE Get() const
	{
		return m_value;
	}

	const TYPE& GetRef() const
	{
		return m_value;
	}
	
	static const int StaticGetTypeID()
	{
		return s_typeID;
	}

private:
	virtual const int GetTypeID() const override
	{
		return s_typeID;
	}
	virtual const bool IsZero() const override
	{
		return Dag2::template IsZero<TYPE>(m_value);


        //Base<T>::template example<int>();
	}

	virtual const bool InternalAssign(const void* const pNewValue) override
	{
		bool bChange = false;
		if (m_value != (*(const TYPE* const )pNewValue))
		{
			bChange = true;
			m_value = (*(const TYPE* const )pNewValue);
		}
		return bChange;
	}

private:
	TYPE m_value;
public:
	static int s_typeID; // = ID::GetID();

};

template< typename TYPE >
int Dag2Value< TYPE >::s_typeID = ID::GetID();
