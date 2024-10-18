#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/action/combat_action_poison_save.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_output.h"
#include "static_lq/combat/combat_enum.h"

StaticLq::CombatActionPosionSave::CombatActionPosionSave(
	ICombatant* in_combatant_receiving_action,
	const int32_t in_poison_risk_factor_after_save,
	const int32_t in_threashold,
	const int32_t in_roll, 
	const int32_t in_stamina
	)
	: _combatant_receiving_action(in_combatant_receiving_action)
	, _poison_risk_factor_after_save(in_poison_risk_factor_after_save)
	, _threashold(in_threashold)
	, _roll(in_roll)
	, _stamina(in_stamina)
{
	// nop
}

void StaticLq::CombatActionPosionSave::PerformAction(
	RandomSequence& in_out_random_sequence,
	const CombatTime& in_combat_time,
	ICombatOutput* in_output
	)
{
	if (in_output && _combatant_receiving_action)
	{
		in_output->CombatantPoisonSave(
			*_combatant_receiving_action, 
			_poison_risk_factor_after_save,
			_threashold,
			_roll,
			_stamina
			);
	}
}