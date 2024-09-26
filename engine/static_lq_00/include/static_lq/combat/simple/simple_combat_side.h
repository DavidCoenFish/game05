#pragma once

#include "static_lq/combat/i_combat_side.h"

namespace static_lq
{

class ICombatSide;
class ICombatant;

class SimpleCombatSide : public ICombatSide
{
public:
    SimpleCombatSide();

private:
    virtual const int GetId() const override;
    void AddCombatant(const std::shared_ptr<ICombatant>& combatant) override;
    void RemoveCombatant(const int combatant_id) override;
    const std::vector<std::shared_ptr<ICombatant>>& GetCombatantList() const override;
    const bool CanContinueCombat() const override;
    const bool IsHostileToSide(const ICombatSide& side) const override;

private:
    const int _id;
};

}