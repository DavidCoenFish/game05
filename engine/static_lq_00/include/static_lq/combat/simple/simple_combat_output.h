#pragma once

#include "static_lq/combat/i_combat_output.h"

class LocaleSystem;

namespace StaticLq
{
class ICombatSide;
class ICombatant;

class SimpleCombatOutput : public ICombatOutput
{
public:
    static void RegisterLocaleSystem(LocaleSystem& in_locale_system);

    typedef std::function<void(const std::string&)> FCallback;
    SimpleCombatOutput(const FCallback& in_log, const std::shared_ptr<LocaleSystem>& in_locale_system);

private:
    virtual void SetCombatStart() override;
    virtual void CombatantAdded(ICombatant& in_combatant, ICombatSide& in_side) override;
    virtual void CombatantRemoved(ICombatant& in_combatant, ICombatSide& in_side) override;

    virtual void SetTurnSegment(const int32_t in_turn, const int32_t in_segment) override;

    //virtual void RecordAttackRoll(const int turn, const int segment) = 0;

    virtual void SetCombatEnd(const std::vector<std::shared_ptr<ICombatSide>>& in_sides_able_to_continue) override;

private:
    FCallback _log;
    std::shared_ptr<LocaleSystem> _locale_system;
};

}
