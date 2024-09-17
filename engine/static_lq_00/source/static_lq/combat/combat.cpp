#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/combat.h"
#include "static_lq/combat/i_combat_output.h"
#include "static_lq/combat/i_combat_side.h"
#include "static_lq/combat/i_combat_topology.h"
#include "static_lq/random_sequence.h"

class CombatImplementation
{
public:
    CombatImplementation(
        const std::shared_ptr<static_lq::ICombatOutput>& combat_output,
        const std::shared_ptr<static_lq::RandomSequence>& random_sequence,
        const std::shared_ptr<static_lq::ICombatTopology>& combatant_topology
        )
        : m_combat_output(combat_output)
        , m_random_sequence(random_sequence)
        , m_combatant_topology(combatant_topology)
        {
            // nop
        }

    void AddSide(const std::shared_ptr<static_lq::ICombatSide>& side)
    {
        // check for duplicates
        std::vector<std::shared_ptr<static_lq::ICombatSide>>::iterator found = std::find(m_side_array.begin(), m_side_array.end(), &side);
        if (found != m_side_array.end())
        {
            DSC_ASSERT_ALWAYS("AddSide side duplicate");
            return;
        }

        m_side_array.push_back(side);
    }

    void RemoveSide(const static_lq::ICombatSide& side)
    {
        //std::vector<std::shared_ptr<static_lq::ICombatSide>> m_side_array;
        std::vector<std::shared_ptr<static_lq::ICombatSide>>::iterator found = std::find(m_side_array.begin(), m_side_array.end(), &side);
        if (found != m_side_array.end())
        {
            m_side_array.erase(found);
        }
    }

    void AddCombatant(const std::shared_ptr<static_lq::ICombatant>& combatant, static_lq::ICombatSide& side)
    {
        std::vector<std::shared_ptr<static_lq::ICombatSide>>::iterator found = std::find(m_side_array.begin(), m_side_array.end(), &side);
        if (found != m_side_array.end())
        {
            (*found)->AddCombatant(combatant);
        }
    }

    void RemoveCombatant(const static_lq::ICombatant& combatant, static_lq::ICombatSide& side)
    {
        std::vector<std::shared_ptr<static_lq::ICombatSide>>::iterator found = std::find(m_side_array.begin(), m_side_array.end(), &side);
        if (found != m_side_array.end())
        {
            (*found)->RemoveCombatant(combatant);
        }
    }

    // return true if combat can continue
    const bool AdvanceTime()
    {
        return false;
    }


private:
    std::shared_ptr<static_lq::ICombatOutput> m_combat_output;
    std::shared_ptr<static_lq::RandomSequence> m_random_sequence;
    std::shared_ptr<static_lq::ICombatTopology> m_combatant_topology;
    
    int m_turn = 0;
    int m_segment = 0;

    std::vector<std::shared_ptr<static_lq::ICombatSide>> m_side_array;
};


static_lq::Combat::Combat(
    const std::shared_ptr<ICombatOutput>& combat_output,
    const std::shared_ptr<RandomSequence>& random_sequence,
    const std::shared_ptr<ICombatTopology>& combatant_topology
    )
{
	m_implementation = std::make_unique<CombatImplementation>(combat_output, random_sequence, combatant_topology);
}

void static_lq::Combat::AddSide(const std::shared_ptr<ICombatSide>& combat_side)
{
    m_implementation->AddSide(combat_side);
}

void static_lq::Combat::RemoveSide(const ICombatSide& side)
{
    m_implementation->RemoveSide(side);
}

void static_lq::Combat::AddCombatant(const std::shared_ptr<ICombatant>& combatant, ICombatSide& side)
{
    m_implementation->AddCombatant(combatant, side);
}

void static_lq::Combat::RemoveCombatant(const ICombatant& combatant, ICombatSide& side)
{
    m_implementation->RemoveCombatant(combatant, side);
}

const bool static_lq::Combat::AdvanceTime()
{
    return m_implementation->AdvanceTime();
}


