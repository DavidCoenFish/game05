#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combat_topology.h"

StaticLq::SimpleCombatTopology::SimpleCombatTopology()
{
	//nop
}

const bool StaticLq::SimpleCombatTopology::CanAttackMellee(const ICombatant& , const ICombatant& )
{
	return true;
}

const bool StaticLq::SimpleCombatTopology::CanAttackRange(const ICombatant& , const ICombatant& )
{
	return true;
}
