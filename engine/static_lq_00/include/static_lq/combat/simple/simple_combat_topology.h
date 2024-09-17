#pragma once

#include "static_lq/combat/i_combat_topology.h"

namespace static_lq
{

class ICombatSide;
class ICombatant;

class SimpleCombatTopology : public ICombatTopology
{
public:
    SimpleCombatTopology();

private:
    const bool CanAttackMellee(const ICombatant& attacker, const ICombatant& receive) override;
    const bool CanAttackRange(const ICombatant& attacker, const ICombatant& receive) override;

};

}
