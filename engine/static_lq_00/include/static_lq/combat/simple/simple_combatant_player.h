#pragma once

#include "static_lq/combat/i_combatant.h"

namespace static_lq
{

class ICombatSide;
class ICombatant;

class SimpleCombatPlayer : public ICombatant
{
public:
    SimpleCombatPlayer();

private:
    const int GetId() const override;
    const std::string GetDisplayName(const LocaleISO_639_1 in_locale) override;
    const bool IsAbleToContinueCombat() override;
    void ApplyDamage(
        const int32_t in_fatigue_damage,
        const int32_t in_physical_damage,
        const int32_t in_paralyzation_damage
        ) override;

private:
    const int _id;

};

}
