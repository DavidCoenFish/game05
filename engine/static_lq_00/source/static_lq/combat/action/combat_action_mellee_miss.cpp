#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/action/combat_action_mellee_miss.h"

#include "static_lq/combat/i_combat_output.h"
#include "static_lq/combat/i_combatant.h"

StaticLq::CombatActionMelleeMiss::CombatActionMelleeMiss(
	ICombatant* in_combatant_performing_action,
	ICombatant* in_combatant_receiving_action,
	const std::string& in_attack_display_name,
	const int32_t in_attack_roll,
	const int32_t in_attack_bonus,
	const int32_t in_defence
	)
	: _combatant_performing_action(in_combatant_performing_action)
	, _combatant_receiving_action(in_combatant_receiving_action)
	, _attack_display_name(in_attack_display_name)
	, _attack_roll(in_attack_roll)
	, _attack_bonus(in_attack_bonus)
	, _defence(in_defence)
{
	// nop
}

void StaticLq::CombatActionMelleeMiss::PerformAction(ICombatOutput& in_output)
{
	in_output.CombatantAttemptMelleeAttack(
		_combatant_performing_action, 
		_combatant_receiving_action,
		_attack_display_name,
		_attack_roll,
		_attack_bonus,
		_defence,
		false
		);
}

