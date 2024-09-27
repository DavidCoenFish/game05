#pragma once

#include "static_lq/combat/i_combatant.h"

namespace static_lq
{

class ICombatSide;
class ICombatant;

class SimpleCombatMonster : public ICombatant
{
public:
    SimpleCombatMonster();

private:
    const int GetId() const override;
    const std::string GetDisplayName(const LocaleISO_639_1 in_locale) override;

private:
    const int _id;
    //DagThreadedCollection
};

}
