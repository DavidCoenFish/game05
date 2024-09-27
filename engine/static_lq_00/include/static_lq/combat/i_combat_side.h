#pragma once

enum class LocaleISO_639_1;

namespace static_lq
{
class ICombatant;

class ICombatSide
{
public:
    static const int MakeNewId();

    virtual ~ICombatSide(){}

    virtual const int GetId() const = 0;

    virtual const std::string GetDisplayName(const LocaleISO_639_1 in_locale) = 0;

    virtual void AddCombatant(const std::shared_ptr<ICombatant>& combatant) = 0;
    virtual void RemoveCombatant(const int combatant_id) = 0;
    virtual const std::vector<std::shared_ptr<ICombatant>>& GetCombatantList() const = 0;

    virtual const bool CanContinueCombat() const = 0;
    virtual const bool IsHostileToSide(const ICombatSide& side) const = 0;

    // get data for success roll? do success roll?

};

}
