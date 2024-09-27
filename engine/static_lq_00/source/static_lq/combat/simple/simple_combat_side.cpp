#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combat_side.h"
#include "static_lq/combat/i_combatant.h"

static_lq::SimpleCombatSide::SimpleCombatSide()
: _id(MakeNewId())
{
}

const int static_lq::SimpleCombatSide::GetId() const 
{
    return _id;
}

const std::string static_lq::SimpleCombatSide::GetDisplayName(const LocaleISO_639_1 in_locale)
{
    in_locale;
    // set locale node
    // get name value
    return "";
}

void static_lq::SimpleCombatSide::AddCombatant(const std::shared_ptr<ICombatant>& in_combatant) 
{
    _combatant_array.push_back(in_combatant);
}

void static_lq::SimpleCombatSide::RemoveCombatant(const int combatant_id) 
{
    auto new_end = std::remove_if(_combatant_array.begin(), _combatant_array.end(),
                                  [combatant_id](const std::shared_ptr<ICombatant>& combatant)
                                  { return combatant->GetId() == combatant_id; });

    _combatant_array.erase(new_end, _combatant_array.end());
}

const std::vector<std::shared_ptr<static_lq::ICombatant>>& static_lq::SimpleCombatSide::GetCombatantList() const
{
    return _combatant_array;
}
const bool static_lq::SimpleCombatSide::CanContinueCombat() const 
{
    return false;
}
const bool static_lq::SimpleCombatSide::IsHostileToSide(const ICombatSide& /*side*/) const 
{
    return true;
}
