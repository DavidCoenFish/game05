#pragma once

namespace static_lq
{
class ICombatant;

class Bestiary
{
public:
    static std::shared_ptr<ICombatant> FactoryDefaultGiantSpider();
    static std::shared_ptr<ICombatant> FactoryDefaultGiantAnt();

};
}
