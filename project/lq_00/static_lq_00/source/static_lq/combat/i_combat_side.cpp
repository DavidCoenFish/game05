#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/i_combat_side.h"

const int32_t StaticLq::ICombatSide::MakeNewId()
{
	static int32_t s_value = 0;
	++s_value;
	return s_value;
}
