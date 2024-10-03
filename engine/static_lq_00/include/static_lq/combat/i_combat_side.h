#pragma once

enum class LocaleISO_639_1;

namespace static_lq
{
class ICombatant;

class ICombatSide
{
public:
    static const int32_t MakeNewId();

    virtual ~ICombatSide(){}

    virtual const int32_t GetId() const = 0;

    virtual const std::string GetDisplayName() = 0;

    virtual void AddCombatant(const std::shared_ptr<ICombatant>& combatant) = 0;
    virtual void RemoveCombatant(const int32_t combatant_id) = 0;
    virtual const std::vector<std::shared_ptr<ICombatant>>& GetCombatantList() const = 0;

    virtual const bool CanContinueCombat() const = 0;
    virtual const bool IsHostileToSide(const ICombatSide& side) const = 0;

    // get data for success roll? do success roll?

};

}
