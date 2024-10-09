#pragma once

#include "static_lq/combat/i_combat_output.h"

class LocaleSystem;

namespace StaticLq
{
class ICombatSide;
class ICombatant;

class SimpleCombatOutput : public ICombatOutput
{
public:
	static void RegisterLocaleSystem(LocaleSystem& in_locale_system);

	typedef std::function<void(const std::string&)> FCallback;
	SimpleCombatOutput(const FCallback& in_log, const std::shared_ptr<LocaleSystem>& in_locale_system);

private:
	virtual void SetCombatStart() override;
	virtual void CombatantAdded(ICombatant& in_combatant, ICombatSide& in_side) override;
	virtual void CombatantRemoved(ICombatant& in_combatant, ICombatSide& in_side) override;

	virtual void SetTurnSegment(const int32_t in_turn, const int32_t in_segment) override;
	virtual void CombatantSetMelleeInitiative(ICombatant& in_combatant, const int32_t in_value) override;
	virtual void CombatantAttemptMelleeAttack(
				ICombatant* const in_combatant_performing_action, 
				ICombatant* const in_combatant_being_hit, 
				const std::string& in_attack,
				const int32_t in_attack_roll,
				const int32_t in_attack_bonus,
				const int32_t in_defence,
				const bool in_hit
				) override;
	virtual void CombatantDamage(
		ICombatant& in_combatant_receive, 
		ICombatant* const in_cause_damage = nullptr, 
		int32_t in_physical_damage_delta = 0,
		int32_t in_fatigue_damage_delta = 0,
		int32_t in_paralyzation_damage_delta = 0
	) override;

	virtual void SetCombatEnd(const std::vector<std::shared_ptr<ICombatSide>>& in_sides_able_to_continue) override;

private:
	FCallback _log;
	std::shared_ptr<LocaleSystem> _locale_system;
};

}
