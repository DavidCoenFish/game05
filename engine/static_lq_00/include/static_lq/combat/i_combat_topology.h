#pragma once

namespace static_lq
{
class ICombatant;

class ICombatTopology
{
public:
    virtual ~ICombatTopology(){}

    virtual const bool CanAttackMellee(const ICombatant& attacker, const ICombatant& receive) = 0;
    virtual const bool CanAttackRange(const ICombatant& attacker, const ICombatant& receive) = 0;

};

}
