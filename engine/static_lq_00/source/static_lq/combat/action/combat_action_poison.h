#pragma once

#include "static_lq/combat/i_combat_action.h"

namespace StaticLq
{
class ICombatant;
class ICombatOutput;

class CombatActionPosion : public ICombatAction
{
public:
	explicit CombatActionPosion(
		ICombatant* in_combatant_performing_action = nullptr,
		ICombatant* in_combatant_receiving_action = nullptr
		// posion type [toxin, hallucinogen, venom]
		// const int32_t in_risk_factor = 0 //strength, dose count
		// luck roll threashold
		// luck roll threashold tooltip
		);

private:
	void PerformAction(
		const CombatTime& in_combat_time,
		ICombatOutput* in_output
		) override;

private:
	ICombatant* _combatant_performing_action = nullptr;
	ICombatant* _combatant_receiving_action = nullptr;


};

}
