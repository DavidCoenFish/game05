#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/effect/combat_effect_damage.h"

#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_output.h"

StaticLq::CombatEffectDamage::CombatEffectDamage(
	const int32_t in_physical_damage_delta,
	const int32_t in_severity_damage_delta,
	const int32_t in_fatigue_damage_delta,
	const int32_t in_paralyzation_damage_delta
	)
	: _physical_damage_delta(in_physical_damage_delta)
	, _severity_damage_delta(in_severity_damage_delta)
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
	const CombatTime& in_combat_time,
	ICombatant& in_target,
	ICombatOutput* in_output
	)
{
	// the advantage of implementing this here is that it is in one place, not in the many implementations of ICombatant::ApplyDamageDelta
	// todo: faith damage, is magical damage, is ImuneMagic? is Imune non magic?
	const int32_t absoption = in_target.GetValue(StaticLq::CombatEnum::CombatantValue::TAbsorption);
	const int32_t susceptible_severity_damage = in_target.GetValue(StaticLq::CombatEnum::CombatantValue::TSusceptibleSeverityDamage);
	int32_t physical_damage = _physical_damage_delta;
	if (0 != susceptible_severity_damage)
	{
		physical_damage += _severity_damage_delta;
	}
	physical_damage = std::max(1, physical_damage - absoption);

	const int32_t before_damage_alive = in_target.GetValue(StaticLq::CombatEnum::CombatantValue::TIsAlive);

	in_target.ApplyDamageDelta(
		physical_damage,
		_fatigue_damage_delta,
		_paralyzation_damage_delta
		);

	const int32_t health_points = in_target.GetValue(StaticLq::CombatEnum::CombatantValue::THealthPoints);
	const int32_t damage_tollerance = in_target.GetValue(StaticLq::CombatEnum::CombatantValue::TDamageTollerance);
	if (nullptr != in_output)
	{
		in_output->CombatantDamage(
			in_target,
			physical_damage,
			health_points,
			damage_tollerance,
			absoption,
			susceptible_severity_damage,
			_physical_damage_delta,
			_severity_damage_delta,
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

std::shared_ptr<StaticLq::ICombatEffect> StaticLq::CombatEffectDamage::Clone() const
{
	return std::make_shared<CombatEffectDamage>(*this);
}

const bool StaticLq::CombatEffectDamage::ContinuePastEndCombat() const
{
	return false;
}
