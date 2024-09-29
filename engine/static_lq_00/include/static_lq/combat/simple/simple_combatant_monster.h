#pragma once

#include "static_lq/combat/i_combatant.h"

class DagThreadedCollection;

namespace static_lq
{
class SimpleCombatMonster : public ICombatant
{
public:
    SimpleCombatMonster(
        const int in_id,
        const std::string& in_display_name,
        const std::shared_ptr<DagThreadedCollection>& in_dag_collection 
        );

private:
    const int GetId() const override;
    const std::string GetDisplayName(const LocaleISO_639_1 in_locale) override;

private:
    const int _id;
    const std::string _display_name;
    std::shared_ptr<DagThreadedCollection> _dag_collection;

    //DagThreadedCollection
};

}
