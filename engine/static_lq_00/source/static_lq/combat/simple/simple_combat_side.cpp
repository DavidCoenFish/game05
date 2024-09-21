#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combat_side.h"
#include "static_lq/combat/i_combatant.h"

static_lq::SimpleCombatSide::SimpleCombatSide()
: _id(MakeNewId())
{
}

void static_lq::SimpleCombatSide::AddCombatant(const std::shared_ptr<ICombatant>& /*combatant*/) {}
void static_lq::SimpleCombatSide::RemoveCombatant(const int /*combatant_id*/) {}
const std::vector<std::shared_ptr<static_lq::ICombatant>>& static_lq::SimpleCombatSide::GetCombatantList() const
{
    static std::vector<std::shared_ptr<ICombatant>> s_data;
    return s_data;
}
const bool static_lq::SimpleCombatSide::CanContinueCombat() const 
{
    return false;
}
const bool static_lq::SimpleCombatSide::IsHostileToSide(const ICombatSide& /*side*/) const 
{
    return true;
}
