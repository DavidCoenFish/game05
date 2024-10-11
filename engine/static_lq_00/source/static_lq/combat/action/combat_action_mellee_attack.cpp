#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/action/combat_action_mellee_attack.h"

#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combat_output.h"
#include "static_lq/combat/i_combatant.h"

StaticLq::CombatActionMelleeAttack::CombatActionMelleeAttack(
	ICombatant* in_combatant_performing_action,
	ICombatant* in_combatant_receiving_action,
	const int32_t in_physical_damage_delta,
	const int32_t in_severity_damage_delta,
	const int32_t in_fatigue_damage_delta,
	const int32_t in_paralyzation_damage_delta,
	const std::string& in_attack_display_name,
	const int32_t in_attack_roll,
	const int32_t in_attack_bonus,
	const int32_t in_defence
	)
	: _combatant_performing_action(in_combatant_performing_action)
	, _combatant_receiving_action(in_combatant_receiving_action)
	, _physical_damage_delta(in_physical_damage_delta)
	, _severity_damage_delta(in_severity_damage_delta)
	, _fatigue_damage_delta(in_fatigue_damage_delta)
	, _paralyzation_damage_delta(in_paralyzation_damage_delta)
	, _attack_display_name(in_attack_display_name)
	, _attack_roll(in_attack_roll)
	, _attack_bonus(in_attack_bonus)
	, _defence(in_defence)
{
	// nop
}

void StaticLq::CombatActionMelleeAttack::PerformAction(ICombatOutput& in_output)
{
	in_output.CombatantAttemptMelleeAttack(
		_combatant_performing_action, 
		_combatant_receiving_action,
		_attack_display_name,
		_attack_roll,
		_attack_bonus,
		_defence,
		true
		);

	if (_combatant_receiving_action)
	{
		_combatant_receiving_action->ApplyDamageDelta(
			&in_output,
			_physical_damage_delta,
			_severity_damage_delta,
			_fatigue_damage_delta,
			_paralyzation_damage_delta
			);
	}
}

