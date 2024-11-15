#pragma once

#include "static_lq/combat/i_combat_action.h"
#include "static_lq/combat/combat_time.h"
#include "static_lq/bestiary/bestiary_enum.h"

namespace StaticLq
{
class ICombatant;
class ICombatOutput;

/*
reminder, perform action should not query state at all, as the idea is to gather the actions, and at that point in time to query state.
once we are performing the actions, state may have been changed by other action order of operations, which is not what we want.

ie, two creatures strike each other at the same time.
we gather the action data from both.
we then apply the action to both.

todo: make a CombatActionPosionFailed for posion not effecting someone
*/
class CombatActionPosion : public ICombatAction
{
public:
	explicit CombatActionPosion(
		ICombatant* in_combatant_performing_action = nullptr,
		ICombatant* in_combatant_receiving_action = nullptr,
		const BestiaryEnum::PoisonType in_poison_type = BestiaryEnum::PoisonType::TDefault,
		const int32_t in_poison_risk_factor = 0,
		const CombatTime& in_trigger_time = CombatTime()
		);

private:
	void PerformAction(
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
		ICombatOutput* in_output
		) override;

private:
	ICombatant* _combatant_performing_action = nullptr;
	ICombatant* _combatant_receiving_action = nullptr;
	BestiaryEnum::PoisonType _poison_type = BestiaryEnum::PoisonType::TDefault;
	int32_t _poison_risk_factor = 0;
	CombatTime _trigger_time = CombatTime();
};

}
