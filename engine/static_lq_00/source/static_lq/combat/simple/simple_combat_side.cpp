#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combat_side.h"

#include "common/locale/locale_system.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/name/name_system.h"

std::shared_ptr<StaticLq::ICombatSide> StaticLq::SimpleCombatSide::Factory(NameSystem& in_name_system, LocaleSystem& in_locale_system, const std::vector<std::shared_ptr<ICombatant>>& in_combatant_array)
{
    const int id = ICombatSide::MakeNewId();
    const std::string name_key = in_name_system.GenerateName(StaticLq::NameSystem::GetKeySide(), id, in_locale_system);
    return std::make_shared<SimpleCombatSide>(id, name_key, in_combatant_array);
}

StaticLq::SimpleCombatSide::SimpleCombatSide(
    const int in_id,
    const std::string& in_display_name,
    const std::vector<std::shared_ptr<ICombatant>>& in_combatant_array
    )
: _id(in_id) //MakeNewId())
, _display_name(in_display_name)
, _combatant_array(in_combatant_array)
{
    // nop
}

void StaticLq::SimpleCombatSide::AddCombatant(const std::shared_ptr<ICombatant>& in_combatant) 
{
    _combatant_array.push_back(in_combatant);
}

void StaticLq::SimpleCombatSide::RemoveCombatant(const int combatant_id) 
{
    auto new_end = std::remove_if(_combatant_array.begin(), _combatant_array.end(),
                                  [combatant_id](const std::shared_ptr<ICombatant>& combatant)
                                  { return combatant->GetId() == combatant_id; });

    _combatant_array.erase(new_end, _combatant_array.end());
}

const bool StaticLq::SimpleCombatSide::CanContinueCombat() const 
{
    for (const auto& item : _combatant_array)
    {
        if (1 == item->GetValue(CombatEnum::CombatantValue::TCanContinueCombat))
        {
            return true;
        }
    }
    return false;
}

const bool StaticLq::SimpleCombatSide::IsHostileToSide(const ICombatSide& /*side*/) const 
{
    //todo, take into account combantant intelegence and attacked history
    return true;
}
