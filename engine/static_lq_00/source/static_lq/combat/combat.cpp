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
    : _combat_output(combat_output)
    , _random_sequence(random_sequence)
    , _combatant_topology(combatant_topology)
    {
        // nop
        _combat_output->SetCombatStart();
    }

    void AddSide(const std::shared_ptr<static_lq::ICombatSide>& side)
    {
        // check for duplicates
        auto lambda = [side](const std::shared_ptr<static_lq::ICombatSide>& rhs) { return rhs.get() == side.get();};
        const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);

        if (found != _side_array.end())
        {
            DSC_ASSERT_ALWAYS("AddSide side duplicate");
            return;
        }

        _side_array.push_back(side);
    }

    void RemoveSide(const int side_id)
    {
        auto lambda = [side_id](const std::shared_ptr<static_lq::ICombatSide>& rhs) -> bool{ return rhs->GetId() == side_id;};
        const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);
        if (found != _side_array.end())
        {
            _side_array.erase(found);
        }
    }

    void AddCombatant(const std::shared_ptr<static_lq::ICombatant>& combatant, const int side_id)
    {
        auto lambda = [side_id](const std::shared_ptr<static_lq::ICombatSide>& rhs) { return rhs->GetId() == side_id;};
        const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);
        if (found != _side_array.end())
        {
            (*found)->AddCombatant(combatant);
        }
    }

    void RemoveCombatant(const int combatant_id, const int side_id)
    {
        auto lambda = [side_id](const std::shared_ptr<static_lq::ICombatSide>& rhs) { return rhs->GetId() == side_id;};
        const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);
        if (found != _side_array.end())
        {
            (*found)->RemoveCombatant(combatant_id);
        }
    }

    // return true if combat can continue
    const bool AdvanceTime()
    {
        _segment += 1;
        while (10 <= _segment)
        {
            _segment -= 10;
            _turn += 1;
        }

        _combat_output->SetTurnSegment(_turn, _segment);

        const std::vector<std::shared_ptr<static_lq::ICombatSide>> result = {};
        _combat_output->SetCombatEnd(result);
        return false;
    }

private:
    std::shared_ptr<static_lq::ICombatOutput> _combat_output;
    std::shared_ptr<static_lq::RandomSequence> _random_sequence;
    std::shared_ptr<static_lq::ICombatTopology> _combatant_topology;
    
    int _turn = 0;
    int _segment = 0;

    std::vector<std::shared_ptr<static_lq::ICombatSide>> _side_array;
};

static_lq::Combat::Combat(
    const std::shared_ptr<ICombatOutput>& combat_output,
    const std::shared_ptr<RandomSequence>& random_sequence,
    const std::shared_ptr<ICombatTopology>& combatant_topology
    )
{
	_implementation = std::make_unique<CombatImplementation>(combat_output, random_sequence, combatant_topology);
}

static_lq::Combat::~Combat()
{
    //nop
}

void static_lq::Combat::AddSide(const std::shared_ptr<ICombatSide>& combat_side)
{
    _implementation->AddSide(combat_side);
}

void static_lq::Combat::RemoveSide(const int side_id)
{
    _implementation->RemoveSide(side_id);
}

void static_lq::Combat::AddCombatant(const std::shared_ptr<ICombatant>& combatant, const int side_id)
{
    _implementation->AddCombatant(combatant, side_id);
}

void static_lq::Combat::RemoveCombatant(const int combatant_id, const int side_id)
{
    _implementation->RemoveCombatant(combatant_id, side_id);
}

const bool static_lq::Combat::AdvanceTime()
{
    return _implementation->AdvanceTime();
}


