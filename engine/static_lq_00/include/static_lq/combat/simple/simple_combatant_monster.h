#pragma once

#include "static_lq/combat/i_combatant.h"

class DagThreadedCollection;

namespace static_lq
{
class SimpleCombatMonster : public ICombatant
{
public:
    SimpleCombatMonster(
        const int in_id,
        const std::string& in_display_name,
        const std::shared_ptr<DagThreadedCollection>& in_dag_collection 
        );

private:
    const int GetId() const override;
    const std::string GetDisplayName() override;
    const int GetHealthPoints() override;
    const int GetDefense() override;
    const int GetAttackBonus() override;
    const bool IsAbleToContinueCombat() override;
    const bool IsMellee() override;
    void SetMelleeInititive(const int in_inititive) override;
    const int GetMelleeInititive() override;
    void SetTurnSegment(const int in_turn, const int in_segment) override;
    void SetTargets(
        const std::vector<std::shared_ptr<ICombatant>>& in_team_mellee,
        const std::vector<std::shared_ptr<ICombatant>>& in_team_range,
        const std::vector<std::shared_ptr<ICombatant>>& in_opponent_mellee,
        const std::vector<std::shared_ptr<ICombatant>>& in_opponent_range
        ) override;
    void GatherAction(
        RandomSequence& in_random_sequence,
        std::vector<std::shared_ptr<ICombatAction>>& out_actions
        ) override;
    void ApplyDamage(
        const int32_t in_physical_damage_basic,
        const int32_t in_physical_damage_severity,
        const int32_t in_fatigue_damage,
        const int32_t in_paralyzation_damage
        ) override;

private:
    const int _id;
    const std::string _display_name;
    std::shared_ptr<DagThreadedCollection> _dag_collection;
};

}
