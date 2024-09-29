#pragma once

namespace static_lq
{
enum class SpecialCharacteristics
{
    TDefault = 0,
    //TModerateParalyzingVenom part of attack action 
    TDarkVision, //Arcane Lore spell ‘Squito Sense.
    TAttackCuresDisease
};

struct SpecialCharacteristicData
{
public:
    SpecialCharacteristics _type;
};
}
