#pragma once

#include "static_lq/combat/i_combat_action.h"

namespace StaticLq
{
class ICombatant;
class ICombatOutput;

class CombatActionMelleeAttack : public ICombatAction
{
public:
	explicit CombatActionMelleeAttack(
		ICombatant* in_combatant_performing_action = nullptr,
		ICombatant* in_combatant_receiving_action = nullptr,
		int32_t in_physical_damage_delta = 0,
		int32_t in_fatigue_damage_delta = 0,
		int32_t in_paralyzation_damage_delta = 0
		);

private:
	void PerformAction(ICombatOutput& in_output) override;

private:
	ICombatant* _combatant_performing_action = nullptr;
	ICombatant* _combatant_receiving_action = nullptr;

	// each would get a tooltip?
	int32_t _physical_damage_delta = 0;
	int32_t _fatigue_damage_delta = 0;
	int32_t _paralyzation_damage_delta = 0;
};

}
