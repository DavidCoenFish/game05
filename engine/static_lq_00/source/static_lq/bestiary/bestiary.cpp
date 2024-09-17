#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/bestiary.h"

#include "static_lq/combat/simple/simple_combatant_monster.h"

std::shared_ptr<static_lq::ICombatant> static_lq::Bestiary::FactoryDefaultGiantSpider()
{
    return std::make_shared<SimpleCombatMonster>();
}

std::shared_ptr<static_lq::ICombatant> static_lq::Bestiary::FactoryDefaultGiantAnt()
{
    return std::make_shared<SimpleCombatMonster>();
}
