#pragma once

class IDagValue
{
public:
	virtual ~IDagValue(){};
	virtual const bool IsZero() const = 0;
	virtual const bool Comparison(const IDagValue* const in_rhs) = 0;

};
