#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/effect/combat_effect_stat_change.h"

#include "static_lq/combat/i_combatant.h"
#include "static_lq/random_sequence.h"

StaticLq::CombatEffectStatChange::CombatEffectStatChange(
	// enum stat to change, ie, stamina
	const StaticLq::CombatEnum::CombatantValue in_node,
	const int32_t in_delta
	)
	: _value_key(in_node)
	, _delta(in_delta)
{
	// nop
}

StaticLq::CombatEffectStatChange::~CombatEffectStatChange()
{
	//nop
}

const bool StaticLq::CombatEffectStatChange::Apply(
	RandomSequence& , //in_out_random_sequence,
	const CombatTime& , //in_combat_time,
	ICombatant& in_target,
	ICombatOutput* //in_output
	)
{
	in_target.AddBuff(_value_key, _delta);
	return false;
}

const bool StaticLq::CombatEffectStatChange::ContinuePastEndCombat() const
{
	return false;
}
