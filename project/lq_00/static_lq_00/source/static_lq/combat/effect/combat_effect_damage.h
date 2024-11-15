#pragma once

#include "static_lq/combat/i_combat_effect.h"

namespace StaticLq
{
class ICombatant;
class ICombatOutput;
class CombatTime;

class CombatEffectDamage : public ICombatEffect
{
public:
	// todo: faith damage, need aligment of trigger, need "is magical damage" flag?
	explicit CombatEffectDamage(
		const int32_t in_physical_damage_delta = 0,
		const int32_t in_fatigue_damage_delta = 0,
		const int32_t in_paralyzation_damage_delta = 0
		);
	~CombatEffectDamage();

	virtual const bool Apply(
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
		ICombatant& in_target,
		ICombatOutput* in_output
		) override;
private:
	virtual const bool ContinuePastEndCombat() const override;

private:
	int32_t _physical_damage_delta = 0;
	int32_t _fatigue_damage_delta = 0;
	int32_t _paralyzation_damage_delta = 0;

	/// todo, may need tooltips to document how each value was generated
	// std::shared_ptr<TooltipData> _physical_damage_tooltip;
	// std::shared_ptr<TooltipData> _fatigue_damage_tooltip;
	// std::shared_ptr<TooltipData> _paralyzation_damage_tooltip;

};
}
