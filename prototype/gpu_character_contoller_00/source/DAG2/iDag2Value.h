#pragma once

class iDag2Value
{
public:
	virtual ~iDag2Value(){};
	virtual const int GetTypeID() const = 0;
	virtual const bool IsZero() const = 0;
	virtual const bool InternalAssign(const void* const pNewValue) = 0;
	//virtual const bool IsDifferent(const void* const pValue) = 0;

};
