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

	TDisplayName, // output
	TSelf, // output
	THealthPoints, // output
	TIsAlive, // output
	TIsParalyzed, // output

	TCanContinueCombat, // combat
	TMelleeInitiative, // combat
	TExperencePointValue, // combat
	TCombatLevel, // combat
	TDefense, // combat
	TAttackBonus, // combat
	TRecoveryTime, // combat? should this be part of the attack data?
	TSusceptibleSeverityDamage, // combat, do we track IsLivingCreature, or the flags based on IsLivingCreature, like taking severity damage or poison
	TSusceptiblePoison, // combat, can we be effected by poision, living creatures should be effected?
	TSusceptibleFaithDamage, // combat
	TSusceptibleNonMagicalDamage, // combat
	TSusceptibleMagicalDamage, // combat
	TAbsorption, // combat
	TAlignment, // combat

	// poison dose count is mostly cumulative
	TPoisonDoseCountToxin, 
	TPoisonDoseCountHallucinogen,
	TPoisonDoseCountVenom,
	TPoisonDoseCountKillingVenom,
	TPoisonDoseCountParalyzingVenom,

	// once you fail a luck throw, all subsuquent luck throws fail untill you get poison state reset
	TPoisonSaveFailedToxin, 
	TPoisonSaveFailedHallucinogen,
	TPoisonSaveFailedVenom,
	TPoisonSaveFailedKillingVenom,
	TPoisonSaveFailedParalyzingVenom,

	// do we need an flag for if the atribute is valid, like some monsters not having manual dexterity
	TPhysicalStrength,
	TStamina,
	TAgility,
	TManualDexterity,
	TPerception,
	TWillpower,
	TFaith,

	TSpecies,
	TVariation,

	TDamageTollerance,
	TDamageTolleranceConstant,
	TDamageTolleranceDiceCount,
	TDamageTolleranceDiceSide,

	TDamageSum,
	TDamageFatigue,
	TDamagePhysical,
	TDamageParalyzationSum,
	TDamageParalyzation,

	TRawPhysicalStrength,
	TRawStamina,
	TRawAgility,
	TRawManualDexterity,
	TRawPerception,
	TRawWillpower,
	TRawFaith,

	TBuffPhysicalStrength,
	TBuffStamina,
	TBuffAgility,
	TBuffManualDexterity,
	TBuffPerception,
	TBuffWillpower,
	TBuffFaith,

};

}
}


template <> 
const StaticLq::CombatEnum::CombatantValue EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumFromString(const std::string& in_locale);
template <> 
const std::string EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(const StaticLq::CombatEnum::CombatantValue in_locale);

