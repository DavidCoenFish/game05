#pragma once

#include "static_lq/bestiary/bestiary_enum.h"

enum class LocaleISO_639_1;

/*
so, do we return a localisation key for the display name, or deal with localisation internally (via DAG?)

*/
namespace static_lq
{
class ICombatant
{
public:
    static const int MakeNewId();

    virtual ~ICombatant(){}
    virtual const int GetId() const = 0;

    /// if these values get sourced from a dag node collection, it's GetValue is not const
    virtual const std::string GetDisplayName(const LocaleISO_639_1 in_locale) = 0;
    virtual const bool IsAbleToContinueCombat() = 0;

    //add attribut value of "faith" to basic attack, though if attach weapon is non magical, and target is immune non magical, then no damage is done
    //virtual const bool IsSubjectFaithDamage(const bestiary_enum::Alignment in_alignment_assailant);
    //virtual const bool IsImmuneNonMagicalDamage();
    //difference between the assailant’s overall Attack Roll and the target’s Defense
    //virtual const bool IsSubjectSeverityDamage();

    virtual void ApplyDamage(
        const int32_t in_fatigue_damage,
        const int32_t in_physical_damage,
        const int32_t in_paralyzation_damage
        ) = 0;

    // 
    //virtual void ApplyDamage(
    //    const int32_t in_basic_damage,
    //    const int32_t in_severity_damage, //difference between the assailant’s overall Attack Roll and the target’s Defense
    //    const int32_t in_faith_damage
    //    // posion damage?
    //    // paralise damage?
    //    ) = 0;

    //virtual const bool CanContinueCombat(const std::shared_ptr<ICombatant>& combatant) = 0;
    //set side? no
    //is weapon drawn
    //is weapon mellee
    //is weapon range
    //set inititive delay
    //draw weapon
    //receive damage(output, basic damage, severity damage, faith damage, side)
    //do attack roll(int& roll_raw, int& roll, output, random_sequence)
    //do luck roll
    //do sucess roll
    //get defence
    //can continue 
    //combat level?
    //get attacks
    //get target?
    //do attack
};

}
