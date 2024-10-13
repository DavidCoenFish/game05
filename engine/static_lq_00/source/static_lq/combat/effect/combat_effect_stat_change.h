#pragma once

#include "static_lq/combat/i_combat_effect.h"
#include "static_lq/combat/combat_enum.h"

namespace StaticLq
{
class ICombatant;
class ICombatOutput;
class CombatTime;

class CombatEffectStatChange : public ICombatEffect
{
public:
	explicit CombatEffectStatChange(
		// enum stat to change, ie, stamina
		const StaticLq::CombatEnum::CombatantValue in_buff_attribute,
		const int32_t in_delta = 0
		);
	~CombatEffectStatChange();

private:
	virtual const bool Apply(
		const CombatTime& in_combat_time,
		ICombatant& in_target,
		ICombatOutput* in_output
		) override;
	virtual std::shared_ptr<ICombatEffect> Clone() const override;
	virtual const bool ContinuePastEndCombat() const override;

private:
	// enumn stat
	int32_t _delta;

};
}
