#pragma once

namespace static_lq
{

class ICombatSide;
class ICombatant;

class ICombatOutput
{
public:
    virtual ~ICombatOutput(){}

    virtual void CombatantAdded(const ICombatant& combatant, const ICombatSide& side) = 0;
    virtual void CombatantRemoved(const ICombatant& combatant, const ICombatSide& side) = 0;

    virtual void SetTurnSegment(const int turn, const int segment) = 0;

    //virtual void RecordAttackRoll(const int turn, const int segment) = 0;

    virtual void SetCombatEnd(const std::vector<std::shared_ptr<ICombatSide>>& sides_able_to_continue) = 0;

};

}