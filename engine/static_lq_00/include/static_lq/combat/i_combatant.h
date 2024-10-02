#pragma once

#include "static_lq/bestiary/bestiary_enum.h"

enum class LocaleISO_639_1;

/*
so, do we return a localisation key for the display name, or deal with localisation internally (via DAG?)

*/
namespace static_lq
{
class ICombatAction;
class RandomSequence;

class ICombatant
{
public:
    static const int MakeNewId();

    virtual ~ICombatant(){}
    virtual const int GetId() const = 0;

    /// if these values get sourced from a dag node collection, it's GetValue is not const

    virtual const std::string GetDisplayName() = 0;
    virtual const int GetHealthPoints() = 0;
    virtual const int GetDefense() = 0;
    virtual const int GetAttackBonus() = 0;
    virtual const bool IsAbleToContinueCombat() = 0;
    virtual const bool IsMellee() = 0;
    virtual void SetMelleeInititive(const int in_inititive) = 0;
    virtual const int GetMelleeInititive() = 0;
    virtual void SetTurnSegment(const int in_turn, const int in_segment) = 0;
    virtual void SetTargets(
        const std::vector<std::shared_ptr<ICombatant>>& in_team_mellee,
        const std::vector<std::shared_ptr<ICombatant>>& in_team_range,
        const std::vector<std::shared_ptr<ICombatant>>& in_opponent_mellee,
        const std::vector<std::shared_ptr<ICombatant>>& in_opponent_range
        ) = 0;
    virtual void GatherAction(
        RandomSequence& in_random_sequence,
        std::vector<std::shared_ptr<ICombatAction>>& out_actions
        ) = 0;
    virtual void ApplyDamage(
        const int32_t in_physical_damage_basic,
        const int32_t in_physical_damage_severity,
        const int32_t in_fatigue_damage,
        const int32_t in_paralyzation_damage
        ) = 0;

    //virtual void ApplyAction(const ICombatAction& in_action) = 0;
    //virtual void SetMellee(const bool value) = 0;
    //virtual const bool IsRange() = 0;
    //virtual void SetRange(const bool value) = 0;

    /// if mellee, then the 1d10 at the start of the turn

    /// recovery time (change weapon? surprise? after weapon action?)
    //virtual void AddRecoveryTime(const int in_recovery_time) = 0;
    //virtual void AdvanceTurn() = 0; //const int in_turn_delta, const int in_segment_delta
    //virtual void AdvanceSegment() = 0; //const int in_turn_delta, const int in_segment_delta

    //add attribut value of "faith" to basic attack, though if attach weapon is non magical, and target is immune non magical, then no damage is done
    //virtual const bool IsSubjectFaithDamage(const bestiary_enum::Alignment in_alignment_assailant);
    //virtual const bool IsImmuneNonMagicalDamage();
    //difference between the assailant’s overall Attack Roll and the target’s Defense
    //virtual const bool IsSubjectSeverityDamage();

    //virtual void ApplyDamage(
    //    const int32_t in_fatigue_damage,
    //    const int32_t in_physical_damage,
    //    const int32_t in_paralyzation_damage
    //    ) = 0;

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
