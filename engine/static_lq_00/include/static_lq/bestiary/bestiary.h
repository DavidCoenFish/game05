#pragma once

class LocaleSystem;

namespace static_lq
{
class ICombatant;
class NameSystem;

class Bestiary
{
public:
    static std::shared_ptr<ICombatant> FactoryDefaultGiantSpider(
        NameSystem& in_name_system, 
        LocaleSystem& in_locale_system
        );
    static std::shared_ptr<ICombatant> FactoryDefaultGiantAnt(
        NameSystem& in_name_system, 
        LocaleSystem& in_locale_system
        );

};
}
