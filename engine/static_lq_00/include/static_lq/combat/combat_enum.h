#pragma once

namespace static_lq
{
namespace combat_enum
{
//CombatantValueString? CombatantValueInt?
enum class CombatantValue
{
    TDefault = 0,
    TDisplayName,
    TDamageTollerance,
    THealthPoints,
    TMelleeInitiative,
    //TAlive,
    TCanContinueCombat,
    TAttackBonus,
    TDefense,
    TSusceptibleSeverityDamage,
};

}
}
