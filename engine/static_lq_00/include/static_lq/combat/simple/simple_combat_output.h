#pragma once

#include "static_lq/combat/i_combat_output.h"

namespace static_lq
{

class ICombatSide;
class ICombatant;

class SimpleCombatOutput : public ICombatOutput
{
public:
    typedef std::function<void(const std::string&)> FCallback;
    SimpleCombatOutput(const FCallback& log);


private:
    virtual void CombatantAdded(const ICombatant& combatant, const ICombatSide& side) override;
    virtual void CombatantRemoved(const ICombatant& combatant, const ICombatSide& side) override;

    virtual void SetTurnSegment(const int turn, const int segment) override;

    //virtual void RecordAttackRoll(const int turn, const int segment) = 0;

    virtual void SetCombatEnd(const std::vector<std::shared_ptr<ICombatSide>>& sides_able_to_continue) override;

private:
    FCallback m_log;

};

}
