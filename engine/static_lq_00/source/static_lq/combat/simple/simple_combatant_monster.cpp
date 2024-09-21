#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"

static_lq::SimpleCombatMonster::SimpleCombatMonster()
: _id(MakeNewId())
{
}

const int static_lq::SimpleCombatMonster::GetId() const
{
    return _id;
}
