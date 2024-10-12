#pragma once

#include "static_lq/combat/i_combat_action.h"

namespace StaticLq
{
class ICombatant;
class ICombatOutput;

class CombatActionMelleeMiss : public ICombatAction
{
public:
	explicit CombatActionMelleeMiss(
		ICombatant* in_combatant_performing_action = nullptr,
		ICombatant* in_combatant_receiving_action = nullptr,
		const std::string& in_attack_display_name = {},
		const int32_t in_attack_roll = 0,
		const int32_t in_attack_bonus = 0,
		const int32_t in_defence = 0
		);

private:
	void PerformAction(ICombatOutput* in_output) override;

private:
	ICombatant* _combatant_performing_action = nullptr;
	ICombatant* _combatant_receiving_action = nullptr;

	std::string _attack_display_name = {};
	int32_t _attack_roll = 0;
	int32_t _attack_bonus = 0;
	int32_t _defence = 0;
};

}
