#pragma once

#include "static_lq/combat/i_combatant.h"
#include "static_lq/bestiary/monster_attack_data.h"

class DagThreadedCollection;
class LocaleSystem;
class Tooltip;

namespace StaticLq
{
class ICombatAction;
class CombatTime;
class RandomSequence;

class SimpleCombatMonster : public ICombatant
{
public:
	SimpleCombatMonster(
		const int32_t in_id,
		const std::shared_ptr<DagThreadedCollection>& in_dag_collection,
		const std::vector<MonsterAttackData> in_attack_data
		);

private:
	const int32_t GetId() const override { return _id; }

	const std::string GetDisplayName() override;
	std::shared_ptr<Tooltip> GetDisplayNameTooltip(const LocaleSystem& in_locale_system) override;
	const int32_t GetValue(const CombatEnum::CombatantValue in_key) override;
	std::shared_ptr<TooltipData> GetTooltip(const CombatEnum::CombatantValue in_key, const LocaleSystem& in_locale_system) override;

	void SetValue(const CombatEnum::CombatantValue in_key, const int32_t in_value) override;

	virtual void TriggerEffects(
		const CombatTime& in_combat_time,
		ICombatOutput* in_output
		) override;
	virtual void GatherAction(
		std::vector<std::shared_ptr<ICombatAction>>& out_actions,
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
		const std::vector<std::shared_ptr<ICombatant>>& in_team_mellee,
		const std::vector<std::shared_ptr<ICombatant>>& in_team_range,
		const std::vector<std::shared_ptr<ICombatant>>& in_opponent_mellee,
		const std::vector<std::shared_ptr<ICombatant>>& in_opponent_range
		) override;
	virtual void ApplyEffect(
		const ICombatEffect& in_effect,
		const CombatTime& in_combat_time,
		ICombatOutput* in_output
		) override;
	virtual const bool HasPostCombatEffect() const override;
	virtual void ApplyDamageDelta(
		const int32_t in_physical_damage_delta,
		const int32_t in_fatigue_damage_delta,
		const int32_t in_paralyzation_damage_delta
		) override;

private:
	const int32_t _id;
	std::shared_ptr<DagThreadedCollection> _dag_collection;

	std::vector<MonsterAttackData> _attack_data;

	// do we put all state in the dag collection, or for some simple local state, like attack index, recovery count, keep state out of dag collection?

	int32_t _attack_index = 0;
	int32_t _mellee_attack_recovery = 0;
	int32_t _range_attack_recovery = 0;

	/// effects that are waiting to be applied, waiting to be disabled
	std::vector<std::shared_ptr<ICombatEffect>> _effect_array;
	
};

}
