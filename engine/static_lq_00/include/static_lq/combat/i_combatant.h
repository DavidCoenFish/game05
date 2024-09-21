#pragma once

namespace static_lq
{
class ICombatant
{
public:
    static const int MakeNewId();

    virtual ~ICombatant(){}
    virtual const int GetId() const = 0;

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
