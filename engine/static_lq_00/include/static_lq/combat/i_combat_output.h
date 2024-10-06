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

	virtual void SetCombatEnd(const std::vector<std::shared_ptr<ICombatSide>>& sides_able_to_continue) = 0;

};

}
