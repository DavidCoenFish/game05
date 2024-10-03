#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combatant_player.h"

StaticLq::SimpleCombatPlayer::SimpleCombatPlayer()
: _id(MakeNewId())
{
}

const int StaticLq::SimpleCombatPlayer::GetId() const
{
    return _id;
}

const std::string StaticLq::SimpleCombatPlayer::GetDisplayName(const LocaleISO_639_1 in_locale)
{
    in_locale;
    return "";
}

