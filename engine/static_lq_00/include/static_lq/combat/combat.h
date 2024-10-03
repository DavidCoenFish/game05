#pragma once

class CombatImplementation;

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
        const std::shared_ptr<ICombatOutput>& combat_output,
        const std::shared_ptr<RandomSequence>& random_sequence,
        const std::shared_ptr<ICombatTopology>& combatant_topology
        );
    ~Combat();

    // enum for supprise/ ambush/ readyness?
    void AddSide(const std::shared_ptr<ICombatSide>& combat_side);
    void RemoveSide(const int32_t side_id);

    void AddCombatant(const std::shared_ptr<ICombatant>& combatant, const int32_t side_id);
    void RemoveCombatant(const int32_t combatant_id, const int32_t side_id);

    // return true if combat can continue
    const bool AdvanceTime();

private:
	std::unique_ptr< CombatImplementation > _implementation;
};
}
