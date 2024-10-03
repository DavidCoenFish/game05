#pragma once

#include "static_lq/combat/i_combatant.h"

namespace static_lq
{

class ICombatSide;
class ICombatant;

class SimpleCombatPlayer : public ICombatant
{
public:
    SimpleCombatPlayer(const int32_t in_id);

private:
    const int32_t GetId() const override;

private:
    const int32_t _id;

};

}
