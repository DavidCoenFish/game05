#pragma once

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

    virtual const std::string GetDisplayName(const LocaleISO_639_1 in_locale) = 0;

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
