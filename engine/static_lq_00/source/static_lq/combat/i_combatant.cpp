#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/i_combatant.h"

const int static_lq::ICombatant::MakeNewId()
{
    static int s_value = 0;
    ++s_value;
    return s_value;
}
