#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/action/combat_action_poison.h"
#include "static_lq/combat/effect/combat_effect_poison.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_output.h"
#include "static_lq/combat/combat_enum.h"

StaticLq::CombatActionPosion::CombatActionPosion(
	ICombatant* in_combatant_performing_action,
	ICombatant* in_combatant_receiving_action,
	const BestiaryEnum::PoisonType in_poison_type,
	const int32_t in_poison_risk_factor,
	const CombatTime& in_trigger_time
	)
	: _combatant_performing_action(in_combatant_performing_action)
	, _combatant_receiving_action(in_combatant_receiving_action)
	, _poison_type(in_poison_type)
	, _poison_risk_factor(in_poison_risk_factor)
	, _trigger_time(in_trigger_time)
{
	// nop
}

void StaticLq::CombatActionPosion::PerformAction(
	RandomSequence& in_out_random_sequence,
	const CombatTime& in_combat_time,
	ICombatOutput* in_output
	)
{
	if (in_output && _combatant_receiving_action)
	{
		in_output->CombatantPoisoned(
			*_combatant_receiving_action, 
			_poison_risk_factor,
			_trigger_time
			);
	}

	if (_combatant_receiving_action)
	{
		_combatant_receiving_action->ApplyEffect(
			in_out_random_sequence,
			std::make_shared<CombatEffectPoison>(
				_trigger_time,
				_poison_type,
				_poison_risk_factor
				),
			in_combat_time,
			in_output
			);
	}
}