#pragma once

#include "static_lq/combat/i_combatant.h"

class DagThreadedCollection;

namespace static_lq
{
class SimpleCombatMonster : public ICombatant
{
public:
    SimpleCombatMonster(
        const int32_t in_id,
        const std::string& in_display_name,
        const std::shared_ptr<DagThreadedCollection>& in_dag_collection 
        );

private:
    const int32_t GetId() const override;

    const std::string GetValueString(const combat_enum::CombatantValue in_key) override;
    const int32_t GetValue(const combat_enum::CombatantValue in_key) override;
    void SetValue(const combat_enum::CombatantValue in_key, const int32_t in_value) override;
    std::shared_ptr<TooltipData> GetTooltip(const combat_enum::CombatantValue in_key) override;

    void GatherAction(
        std::vector<std::shared_ptr<ICombatAction>>& out_actions,
        RandomSequence& in_out_random_sequence,
        const int32_t in_turn, 
        const int32_t in_segment,
        const std::vector<std::shared_ptr<ICombatant>>& in_team_mellee,
        const std::vector<std::shared_ptr<ICombatant>>& in_team_range,
        const std::vector<std::shared_ptr<ICombatant>>& in_opponent_mellee,
        const std::vector<std::shared_ptr<ICombatant>>& in_opponent_range
        ) override;

    // you could use SetValue, but there is some logic to not allow negative value totals for damage
    // positive to do damage, negative to heal
    void ApplyDamageDelta(
        const int32_t in_physical_damage_delta,
        const int32_t in_fatigue_damage_delta,
        const int32_t in_paralyzation_damage_delta
        ) override;

private:
    const int32_t _id;
    const std::string _display_name;
    std::shared_ptr<DagThreadedCollection> _dag_collection;
};

}
