#pragma once

#include "static_lq/combat/i_combat_effect.h"
#include "static_lq/combat/combat_enum.h"

namespace StaticLq
{
class ICombatant;
class ICombatOutput;
class CombatTime;
class RandomSequence;

class CombatEffectStatChange : public ICombatEffect
{
public:
	explicit CombatEffectStatChange(
		// enum stat to change, ie, stamina
		const StaticLq::CombatEnum::CombatantValue in_value_key,
		const int32_t in_delta = 0
		);
	~CombatEffectStatChange();

private:
	virtual const bool Apply(
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
		ICombatant& in_target,
		ICombatOutput* in_output
		) override;
	virtual const bool ContinuePastEndCombat() const override;

private:
	StaticLq::CombatEnum::CombatantValue _value_key;
	int32_t _delta;

};
}
