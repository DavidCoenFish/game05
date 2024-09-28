#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/bestiary.h"

#include "common/locale/locale_system.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"
#include "static_lq/name/name_system.h"

std::shared_ptr<static_lq::ICombatant> static_lq::Bestiary::FactoryDefaultGiantSpider(
    NameSystem& in_name_system, 
    LocaleSystem& in_locale_system
    )
{
    const int id = ICombatant::MakeNewId();
    const std::string name_key = in_name_system.GenerateName(static_lq::NameSystem::GetKeyGiantAnt(), id, in_locale_system);
    return std::make_shared<SimpleCombatMonster>(id, name_key);
}

std::shared_ptr<static_lq::ICombatant> static_lq::Bestiary::FactoryDefaultGiantAnt(
    NameSystem& in_name_system, 
    LocaleSystem& in_locale_system
    )
{
    const int id = ICombatant::MakeNewId();
    const std::string name_key = in_name_system.GenerateName(static_lq::NameSystem::GetKeyGiantSpider(), id, in_locale_system);
    return std::make_shared<SimpleCombatMonster>(id, name_key);
}
