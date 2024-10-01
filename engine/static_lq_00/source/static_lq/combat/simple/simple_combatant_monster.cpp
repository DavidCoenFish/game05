#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"

#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"
#include "common/locale/locale_system.h"
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

const std::string static_lq::SimpleCombatMonster::GetDisplayName(const LocaleISO_639_1 in_locale)
{
    in_locale;
    return _display_name;
}

const bool static_lq::SimpleCombatMonster::IsAbleToContinueCombat()
{
    const bool alive = DagThreadedHelper::GetValue<bool>(_dag_collection->GetDagValue(_dag_collection->FindNode(static_lq::Bestiary::GetDagKeyAlive())));
    return alive; // && concious
}

void static_lq::SimpleCombatMonster::ApplyDamage(
    const int32_t in_fatigue_damage,
    const int32_t in_physical_damage,
    const int32_t in_paralyzation_damage
    )
{
    AddDamage(*_dag_collection, static_lq::Bestiary::GetDagKeyFatigueDamage(), in_fatigue_damage);
    AddDamage(*_dag_collection, static_lq::Bestiary::GetDagKeyPhysicalDamage(), in_physical_damage);
    AddDamage(*_dag_collection, static_lq::Bestiary::GetDagKeyParalyzationDamage(), in_paralyzation_damage);
}


