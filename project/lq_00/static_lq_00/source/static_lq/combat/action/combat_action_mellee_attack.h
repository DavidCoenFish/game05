#pragma once

#include "static_lq/combat/i_combat_action.h"
#include "static_lq/combat/i_combat_effect.h"

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
		const std::shared_ptr<ICombatEffect>& in_combat_damage = nullptr,
		const std::string& in_attack_display_name = {},
		const int32_t in_attack_roll = 0,
		const int32_t in_attack_bonus = 0,
		const int32_t in_defence = 0
		);

private:
	virtual void PerformAction(
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
		ICombatOutput* in_output
		) override;

private:
	ICombatant* _combatant_performing_action = nullptr;
	ICombatant* _combatant_receiving_action = nullptr;

	std::shared_ptr<ICombatEffect> _combat_damage = nullptr;

	std::string _attack_display_name = {};
	int32_t _attack_roll = 0;
	int32_t _attack_bonus = 0;
	int32_t _defence = 0;

};

}
