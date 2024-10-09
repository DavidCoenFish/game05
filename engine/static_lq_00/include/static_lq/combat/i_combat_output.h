#pragma once

namespace StaticLq
{

class ICombatSide;
class ICombatant;

class ICombatOutput
{
public:
	virtual ~ICombatOutput(){}

	virtual void SetCombatStart() = 0;
	virtual void CombatantAdded(ICombatant& in_combatant, ICombatSide& in_side) = 0;
	virtual void CombatantRemoved(ICombatant& in_combatant, ICombatSide& in_side) = 0;


	virtual void SetTurnSegment(const int32_t in_turn, const int32_t in_segment) = 0;
	virtual void CombatantSetMelleeInitiative(ICombatant& in_combatant, const int32_t in_value) = 0;
	virtual void CombatantAttemptMelleeAttack(
				ICombatant* const in_combatant_performing_action, 
				ICombatant* const in_combatant_being_hit, 
				const std::string& in_attack,
				const int32_t in_attack_roll,
				const int32_t in_attack_bonus,
				const int32_t in_defence,
				const bool in_hit
				) = 0;

	virtual void CombatantDamage(
		ICombatant& in_combatant_receive, 
		ICombatant* const in_cause_damage = nullptr, 
		int32_t in_physical_damage_delta = 0,
		int32_t in_fatigue_damage_delta = 0,
		int32_t in_paralyzation_damage_delta = 0
	) = 0;

	virtual void SetCombatEnd(const std::vector<std::shared_ptr<ICombatSide>>& sides_able_to_continue) = 0;

};

}
