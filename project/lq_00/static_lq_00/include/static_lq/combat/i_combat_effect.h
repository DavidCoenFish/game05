#pragma once

namespace StaticLq
{
class CombatTime;
class ICombatant;
class ICombatOutput;
class RandomSequence;

// an effect is something like adding damage to a combatant, or setting up poison to trigger in 1d4 turns on a combatant
class ICombatEffect
{
public:
	virtual ~ICombatEffect(){}

	/// return true if we still need to trigger the effect next round, 
	/// affect the effect...
	virtual const bool Apply(
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
		ICombatant& in_target,
		ICombatOutput* in_output
		) = 0;

	/// some effects like poison have delayed effects
	virtual const bool ContinuePastEndCombat() const = 0;

	// No. accumulate via state on the combatant, not in the effect
	/// if you return true, delete input, else keep input and this object
	/// use case, posion of the same time and on the same time segment get accumulated
	//virtual const bool Accumulate(const ICombatEffect& in_input) = 0;
};

}
