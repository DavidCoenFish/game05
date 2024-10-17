#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/effect/combat_effect_damage.h"

#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_output.h"

StaticLq::CombatEffectDamage::CombatEffectDamage(
	const int32_t in_physical_damage_delta,
	const int32_t in_fatigue_damage_delta,
	const int32_t in_paralyzation_damage_delta
	)
	: _physical_damage_delta(in_physical_damage_delta)
	, _fatigue_damage_delta(in_fatigue_damage_delta)
	, _paralyzation_damage_delta(in_paralyzation_damage_delta)
{
	// nop
}

StaticLq::CombatEffectDamage::~CombatEffectDamage()
{
	// nop
}

const bool StaticLq::CombatEffectDamage::Apply(
	RandomSequence&,
	const CombatTime&, // in_combat_time,
	ICombatant& in_target,
	ICombatOutput* in_output
	)
{
	const int32_t before_damage_alive = in_target.GetValue(StaticLq::CombatEnum::CombatantValue::TIsAlive);

	in_target.ApplyDamageDelta(
		_physical_damage_delta,
		_fatigue_damage_delta,
		_paralyzation_damage_delta
		);

	const int32_t health_points = in_target.GetValue(StaticLq::CombatEnum::CombatantValue::THealthPoints);
	const int32_t damage_tollerance = in_target.GetValue(StaticLq::CombatEnum::CombatantValue::TDamageTollerance);
	if (nullptr != in_output)
	{
		in_output->CombatantDamage(
			in_target,
			health_points,
			damage_tollerance,
			_physical_damage_delta,
			_fatigue_damage_delta,		
			_paralyzation_damage_delta
			);
	}

	const int32_t after_damage_alive = in_target.GetValue(StaticLq::CombatEnum::CombatantValue::TIsAlive);

	if ((before_damage_alive == 1) && (after_damage_alive == 0) && (nullptr != in_output))
	{
		in_output->CombatantDied(in_target);
	}

	return false;
}

const bool StaticLq::CombatEffectDamage::ContinuePastEndCombat() const
{
	return false;
}
