#pragma once

class IDagThreadedValue
{
public:
	virtual ~IDagThreadedValue(){};
	//virtual const int GetTypeID() const = 0;
	virtual const bool IsZero() const = 0;
	//virtual const bool InternalAssign(const void* const pNewValue) = 0;
	virtual const bool Comparison(const IDagThreadedValue* const in_rhs) = 0;

};
