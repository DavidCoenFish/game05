#pragma once

#include "common/util/enum_soft_bind.h"

namespace StaticLq
{
namespace CombatEnum
{

// using this as both as the DagCollection node key, and the local system dag node display value key

/// these are the keys that the Combat system expects a Combatant to be able to provide
/// used for combat (values) and output (tooltips)
enum class CombatantValue
{
	TDefault = 0,

	TSelf, // output

	TCanContinueCombat, // combat
	TMelleeInitiative, // combat
	TDefense, // combat
	TAttackBonus, // combat
	TSusceptibleSeverityDamage, // combat
	// TIsMagicalDamage or is this part of the attack data
	TSusceptibleFaithDamage, // combat
	TAlignment, // combat
	TSusceptibleNonMagicalDamage, // combat
};

}
}


template <> 
const StaticLq::CombatEnum::CombatantValue EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumFromString(const std::string& in_locale);
template <> 
const std::string EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(const StaticLq::CombatEnum::CombatantValue in_locale);

