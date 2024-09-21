#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combatant_player.h"

static_lq::SimpleCombatPlayer::SimpleCombatPlayer()
: _id(MakeNewId())
{
}

const int static_lq::SimpleCombatPlayer::GetId() const
{
    return _id;
}
