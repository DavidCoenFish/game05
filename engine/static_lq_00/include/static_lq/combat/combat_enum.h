#pragma once

#include "common/util/enum_soft_bind.h"

namespace StaticLq
{
namespace CombatEnum
{

// using this as both as the DagCollection node key, and the local system dag node display value key

/// these are the keys that the Combat system expects a Combatant to be able to provide
/// used for combat and output (tooltips)
enum class CombatantValue
{
    TDefault = 0,
    TSelf,
    TName,
    TSpecies,
    TVariation,

    TDamageTollerance,
    TDamageSum,
    TDamageFatigue,
    TDamagePhysical,
    TDamageParalyzation,

    THealthPoints,
    TMelleeInitiative,

    TAlive,
    TCanContinueCombat,
    TAttackBonus,
    TDefense,
    TSusceptibleSeverityDamage,
};

}
}


template <> 
const StaticLq::CombatEnum::CombatantValue EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumFromString(const std::string& in_locale);
template <> 
const const std::string EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(const StaticLq::CombatEnum::CombatantValue in_locale);

