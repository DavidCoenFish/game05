#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"

#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"
#include "common/locale/locale_system.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/bestiary/bestiary.h"
#include "static_lq/name/name_system.h"

namespace
{
void AddDamage(DagThreadedCollection& in_dag_collection, const std::string& in_dag_key, const int32_t in_damage)
{
    auto node_key = in_dag_collection.FindNode(in_dag_key);
    const int32_t value = DagThreadedHelper::GetValue<int32_t>(in_dag_collection.GetDagValue(node_key));
    // in_damage can be positive or negative, but the total damage of a type, can not be negative
    const int32_t result = std::max(0, value + in_damage);

    in_dag_collection.SetDagValue(node_key, DagThreadedHelper::CreateDagValue<int32_t>(result));
}

}

static_lq::SimpleCombatMonster::SimpleCombatMonster(
    const int in_id,
    const std::string& in_display_name,
    const std::shared_ptr<DagThreadedCollection>& in_dag_collection 
    )
: _id(in_id)
, _display_name(in_display_name)
, _dag_collection(in_dag_collection)
{
}

const int static_lq::SimpleCombatMonster::GetId() const
{
    return _id;
}

const std::string static_lq::SimpleCombatMonster::GetValueString(const combat_enum::CombatantValue in_key) 
{
    switch(in_key)
    {
    case combat_enum::CombatantValue::TDisplayName:
        return _display_name;
    default:
        break;
    }
    const int value = GetValue(in_key);
    return std::to_string(value);
}
const int static_lq::SimpleCombatMonster::GetValue(const combat_enum::CombatantValue in_key) 
{
    switch(in_key)
    {
    case combat_enum::CombatantValue::TAttackBonus:
        return DagThreadedHelper::GetValue<int32_t>(_dag_collection->GetDagValue(_dag_collection->FindNode(static_lq::Bestiary::GetDagKeyAlive())));
    default:
        break;
    }
    return 0;
}

void static_lq::SimpleCombatMonster::SetValue(const combat_enum::CombatantValue in_key, const int in_value) 
{
    switch(in_key)
    {
    case combat_enum::CombatantValue::TAttackBonus:
        _dag_collection->SetDagValue(
            _dag_collection->FindNode(static_lq::Bestiary::GetDagKeyAttackBonus()), 
            DagThreadedHelper::CreateDagValue<int32_t>(in_value)
            );
        break;
    default:
        break;
    }
    return 0;
}

std::shared_ptr<TooltipData> static_lq::SimpleCombatMonster::GetTooltip(const combat_enum::CombatantValue in_key) {}

void static_lq::SimpleCombatMonster::GatherAction(
    std::vector<std::shared_ptr<ICombatAction>>& out_actions,
    RandomSequence& in_out_random_sequence,
    const int in_turn, 
    const int in_segment,
    const std::vector<std::shared_ptr<ICombatant>>& in_team_mellee,
    const std::vector<std::shared_ptr<ICombatant>>& in_team_range,
    const std::vector<std::shared_ptr<ICombatant>>& in_opponent_mellee,
    const std::vector<std::shared_ptr<ICombatant>>& in_opponent_range
    ) {}

// you could use SetValue, but there is some logic to not allow negative value totals for damage
// positive to do damage, negative to heal
void static_lq::SimpleCombatMonster::ApplyDamageDelta(
    const int32_t in_physical_damage_delta,
    const int32_t in_fatigue_damage_delta,
    const int32_t in_paralyzation_damage_delta
    )
{
}

//const std::string static_lq::SimpleCombatMonster::GetDisplayName()
//{
//    return _display_name;
//}
//
//const int static_lq::SimpleCombatMonster::GetHealthPoints()
//{
//}
//
//const int static_lq::SimpleCombatMonster::GetDamageTollerance()
//{
//}
//
//const int static_lq::SimpleCombatMonster::GetDefense()
//{
//}
//
//const int static_lq::SimpleCombatMonster::GetAttackBonus()
//{
//}
//
//const bool static_lq::SimpleCombatMonster::IsAbleToContinueCombat()
//{
//    const bool alive = DagThreadedHelper::GetValue<bool>(_dag_collection->GetDagValue(_dag_collection->FindNode(static_lq::Bestiary::GetDagKeyAlive())));
//    return alive; // && concious
//}
//
//const bool static_lq::SimpleCombatMonster::IsMellee()
//const bool static_lq::SimpleCombatMonster::IsSusceptibleSeverityDamage()
//
//void static_lq::SimpleCombatMonster::SetMelleeInititive(const int in_inititive)
//const int static_lq::SimpleCombatMonster::GetMelleeInititive() {}
//void static_lq::SimpleCombatMonster::GatherAction(
//    std::vector<std::shared_ptr<ICombatAction>>& out_actions,
//    RandomSequence& in_out_random_sequence,
//    const int in_turn, 
//    const int in_segment,
//    const std::vector<std::shared_ptr<ICombatant>>& in_team_mellee,
//    const std::vector<std::shared_ptr<ICombatant>>& in_team_range,
//    const std::vector<std::shared_ptr<ICombatant>>& in_opponent_mellee,
//    const std::vector<std::shared_ptr<ICombatant>>& in_opponent_range
//    )
//
//void static_lq::SimpleCombatMonster::ApplyDamage(
//    const int32_t in_fatigue_damage,
//    const int32_t in_physical_damage,
//    const int32_t in_paralyzation_damage
//    )
//{
//    AddDamage(*_dag_collection, static_lq::Bestiary::GetDagKeyFatigueDamage(), in_fatigue_damage);
//    AddDamage(*_dag_collection, static_lq::Bestiary::GetDagKeyPhysicalDamage(), in_physical_damage);
//    AddDamage(*_dag_collection, static_lq::Bestiary::GetDagKeyParalyzationDamage(), in_paralyzation_damage);
//}
//
//
