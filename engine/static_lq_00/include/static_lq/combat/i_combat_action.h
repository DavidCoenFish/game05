#pragma once

class RandomSequence;

namespace StaticLq
{
class CombatTime;
class ICombatOutput;

/// an action is something we perform in combat, like attacking someone, poisoning someone
/// an action can apply/add an effect on a combatant
class ICombatAction
{
public:
	virtual ~ICombatAction(){}

	virtual void PerformAction(
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
		ICombatOutput* in_output
		) = 0;

};

}
