#pragma once

class IDagThreadedValue
{
public:
	virtual ~IDagThreadedValue(){};
	virtual const bool IsZero() const = 0;
	virtual const bool Comparison(const IDagThreadedValue* const in_rhs) = 0;

};
