#pragma once

#include "static_lq/combat/i_combatant.h"

class DagThreadedCollection;
class LocaleSystem;

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
		const std::shared_ptr<DagThreadedCollection>& in_dag_collection 
		);

private:
	const int32_t GetId() const override { return _id; }

	//const std::string GetDisplayName() override;
	//std::shared_ptr<TooltipData> GetDisplayNameTooltip(const int32_t in_level = 0) override;
	const int32_t GetValue(const CombatEnum::CombatantValue in_key) override;
	std::shared_ptr<TooltipData> GetTooltip(const CombatEnum::CombatantValue in_key, const LocaleSystem& in_locale_system) override;

	void SetValue(const CombatEnum::CombatantValue in_key, const int32_t in_value) override;

	void GatherAction(
		std::vector<std::shared_ptr<ICombatAction>>& out_actions,
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
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
	std::shared_ptr<DagThreadedCollection> _dag_collection;
};

}
