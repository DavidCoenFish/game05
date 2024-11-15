#pragma once

#include "static_lq/combat/i_combat_action.h"

namespace StaticLq
{
class ICombatant;
class ICombatOutput;

/*
*/
class CombatActionPosionSave : public ICombatAction
{
public:
	explicit CombatActionPosionSave(
		ICombatant* in_combatant_receiving_action = nullptr,
		const int32_t in_poison_risk_factor_after_save = 0,
		const int32_t in_threashold = 0,
		const int32_t in_roll = 0, 
		const int32_t in_stamina = 0
		);

private:
	void PerformAction(
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
		ICombatOutput* in_output
		) override;

private:
	ICombatant* _combatant_receiving_action = nullptr;
	int32_t _poison_risk_factor_after_save = 0;
	int32_t _threashold = 0;
	int32_t _roll = 0;
	int32_t _stamina = 0;

};

}
