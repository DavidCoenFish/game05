#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/action/combat_action_mellee_attack.h"

#include "static_lq/combat/i_combat_output.h"
#include "static_lq/combat/i_combatant.h"

StaticLq::CombatActionMelleeAttack::CombatActionMelleeAttack(
	ICombatant* in_combatant_performing_action,
	ICombatant* in_combatant_receiving_action,
	int32_t in_physical_damage_delta,
	int32_t in_fatigue_damage_delta,
	int32_t in_paralyzation_damage_delta
	)
	: _combatant_performing_action(in_combatant_performing_action)
	, _combatant_receiving_action(in_combatant_receiving_action)
	, _physical_damage_delta(in_physical_damage_delta)
	, _fatigue_damage_delta(in_fatigue_damage_delta)
	, _paralyzation_damage_delta(in_paralyzation_damage_delta)
{
	// nop
}

void StaticLq::CombatActionMelleeAttack::PerformAction(ICombatOutput& in_output)
{
	if (_combatant_receiving_action)
	{
		in_output.CombatantDamage(
			*_combatant_receiving_action,
			_combatant_performing_action, 
			_physical_damage_delta,
			_fatigue_damage_delta,
			_paralyzation_damage_delta
			);

		_combatant_receiving_action->ApplyDamageDelta(
			_physical_damage_delta,
			_fatigue_damage_delta,
			_paralyzation_damage_delta
			);
	}
}

