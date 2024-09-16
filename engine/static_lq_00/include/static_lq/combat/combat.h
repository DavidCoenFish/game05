#pragma once

namespace static_lq
{
class ICombatant;
class ICombatOutput;
class ICombatSide;
class ICombatTopology;
class RandomSequence;

class Combat
{
public:
    Combat(
        const std::shared_ptr<ICombatTopology>& combatant_topology,
        const std::shared_ptr<ICombatOutput>& combat_output,
        const std::shared_ptr<RandomSequence>& random_sequence
        );

    // enum for supprise/ ambush/ readyness?
    void AddSide(const std::shared_ptr<ICombatSide>& combat_side);
    void RemoveSide(const ICombatSide& side);

    void AddCombatant(const std::shared_ptr<ICombatant>& combatant, ICombatSide& side);
    void RemoveCombatant(const std::shared_ptr<ICombatant>& combatant, ICombatSide& side);

    // return true if combat can continue
    const bool AdvanceTime();

};
}
