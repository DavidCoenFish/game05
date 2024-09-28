#pragma once

#include "static_lq/combat/i_combat_side.h"

class LocaleSystem;

namespace static_lq
{
class ICombatSide;
class ICombatant;
class NameSystem;

class SimpleCombatSide : public ICombatSide
{
public:
    static std::shared_ptr<ICombatSide> Factory(
        NameSystem& in_name_system, 
        LocaleSystem& in_locale_system, 
        const std::vector<std::shared_ptr<ICombatant>>& in_combatant_array = {}
        );

    SimpleCombatSide(
        const int in_id,
        const std::string& in_display_name, //up to project if this is locale key or raw value
        const std::vector<std::shared_ptr<ICombatant>>& in_combatant_array = {}
        );

private:
    const int GetId() const override;
    const std::string GetDisplayName(const LocaleISO_639_1 in_locale) override;
    void AddCombatant(const std::shared_ptr<ICombatant>& combatant) override;
    void RemoveCombatant(const int combatant_id) override;
    const std::vector<std::shared_ptr<ICombatant>>& GetCombatantList() const override;
    const bool CanContinueCombat() const override;
    const bool IsHostileToSide(const ICombatSide& side) const override;

private:
    const int _id;
    const std::string _display_name;
    std::vector<std::shared_ptr<ICombatant>> _combatant_array;
};

}
