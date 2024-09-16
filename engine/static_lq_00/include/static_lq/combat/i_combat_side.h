#pragma once

namespace static_lq
{
class ICombatant;

class ICombatSide
{
public:
    virtual void AddCombatant(const std::shared_ptr<ICombatant>& combatant) = 0;
    virtual void RemoveCombatant(const ICombatant& combatant) = 0;
    virtual const std::vector<std::shared_ptr<ICombatant>>& GetCombatantList() const = 0;

    virtual const bool CanContinueCombat() const = 0;
    virtual const bool IsHostileToSide(const ICombatSide& side) const = 0;

    // get data for success roll? do success roll?

};

}
