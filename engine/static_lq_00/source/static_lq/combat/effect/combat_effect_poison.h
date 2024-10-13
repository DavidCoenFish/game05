#pragma once

#include "static_lq/combat/i_combat_effect.h"
#include "static_lq/combat/combat_time.h"
#include "static_lq/bestiary/bestiary_enum.h"

namespace StaticLq
{
class ICombatant;
class ICombatOutput;
class CombatTime;

class CombatEffectPoison : public ICombatEffect
{
public:
	explicit CombatEffectPoison(
		const CombatTime& in_trigger_time = CombatTime(),
		const BestiaryEnum::PoisonType in_type = BestiaryEnum::PoisonType::TDefault,
		const int32_t in_risk_factor = 0
		);
	~CombatEffectPoison();

private:
	virtual const bool Apply(
		const CombatTime& in_combat_time,
		ICombatant& in_target,
		ICombatOutput* in_output
		) override;
	virtual std::shared_ptr<ICombatEffect> Clone() const override;
	virtual const bool ContinuePastEndCombat() const override;

private:
	CombatTime _trigger_time;
	BestiaryEnum::PoisonType _type;
	int32_t _risk_factor;

	/// todo, may need tooltips to document how each value was generated

};
}
