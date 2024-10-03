#pragma once

namespace StaticLq
{
class ICombatOutput;

class ICombatAction
{
public:
    virtual ~ICombatAction(){}

    virtual void PerformAction(const ICombatOutput& in_output) = 0;

};

}
