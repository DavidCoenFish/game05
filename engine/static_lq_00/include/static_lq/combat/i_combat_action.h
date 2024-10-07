#pragma once

namespace StaticLq
{
class ICombatOutput;

class ICombatAction
{
public:
	virtual ~ICombatAction(){}

	virtual void PerformAction(ICombatOutput& in_output) = 0;

};

}
