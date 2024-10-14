#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/effect/combat_effect_poison.h"

#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_output.h"


StaticLq::CombatEffectPoison::CombatEffectPoison(
	const CombatTime& in_trigger_time,
	const BestiaryEnum::PoisonType in_type,
	const int32_t in_risk_factor
	)
	: _trigger_time(in_trigger_time)
	, _type(in_type)
	, _risk_factor(in_risk_factor)
{
	// nop
}

const bool StaticLq::CombatEffectPoison::Apply(
	RandomSequence& in_out_random_sequence,
	const CombatTime& in_combat_time,
	ICombatant& in_target,
	ICombatOutput* in_output
	)
{
	if (CombatTime::Equivalent(in_combat_time, _trigger_time))
	{
		//add the dose

		//apply damage, status effects
		// TODO: need random sequence

		return false;
	}

	return true;
}

const bool StaticLq::CombatEffectPoison::ContinuePastEndCombat() const
{
	return true;
}

