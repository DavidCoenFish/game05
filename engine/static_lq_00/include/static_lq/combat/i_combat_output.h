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
	virtual void CombatantAdded(ICombatant& combatant, ICombatSide& side) = 0;
	virtual void CombatantRemoved(ICombatant& combatant, ICombatSide& side) = 0;


	virtual void SetTurnSegment(const int32_t turn, const int32_t segment) = 0;

	//virtual void RecordAttackRoll(const int turn, const int segment) = 0;

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
