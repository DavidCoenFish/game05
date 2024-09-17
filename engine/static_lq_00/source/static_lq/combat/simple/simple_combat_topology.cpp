#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combat_topology.h"

static_lq::SimpleCombatTopology::SimpleCombatTopology()
{
    //nop
}

const bool static_lq::SimpleCombatTopology::CanAttackMellee(const ICombatant& , const ICombatant& )
{
    return true;
}

const bool static_lq::SimpleCombatTopology::CanAttackRange(const ICombatant& , const ICombatant& )
{
    return true;
}
