#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"

#include "common/locale/locale_system.h"
#include "static_lq/name/name_system.h"
#include "static_lq/combat/i_combatant.h"

static_lq::SimpleCombatMonster::SimpleCombatMonster(
    const int in_id,
    const std::string& in_display_name
    )
: _id(in_id)
, _display_name(in_display_name)
{
}

const int static_lq::SimpleCombatMonster::GetId() const
{
    return _id;
}

const std::string static_lq::SimpleCombatMonster::GetDisplayName(const LocaleISO_639_1 in_locale)
{
    in_locale;
    return _display_name;
}

