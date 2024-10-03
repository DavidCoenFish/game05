#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/combat.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combat_action.h"
#include "static_lq/combat/i_combat_output.h"
#include "static_lq/combat/i_combat_side.h"
#include "static_lq/combat/i_combat_topology.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/random_sequence.h"

namespace
{
void GatherStillFunctionalSides(std::vector<std::shared_ptr<static_lq::ICombatSide>>& out_still_functional_sides, const std::vector<std::shared_ptr<static_lq::ICombatSide>>& in_side_array)
{
    for (const auto& item : in_side_array)
    {
        if (true == item->CanContinueCombat())
        {
            out_still_functional_sides.push_back(item);
        }
    }
}

const bool TestSidesAntagonistic(const std::vector<std::shared_ptr<static_lq::ICombatSide>>& in_side_array)
{
    const int32_t count = static_cast<int32_t>(in_side_array.size());
    for (int32_t index = 0; index < count; ++index)
    {
        for (int32_t sub_index = 0; sub_index < count; ++sub_index)
        {
            if (index == sub_index)
            {
                continue;
            }

            if (in_side_array[index]->IsHostileToSide(*in_side_array[sub_index]))
            {
                return true;
            }
        }
    }
    return false;
}

void SetMelleeInititive(static_lq::ICombatant& in_combatant, static_lq::RandomSequence& in_random_sequence)
{
    const int32_t value = in_random_sequence.GenerateDice(10);
    in_combatant.SetValue(static_lq::combat_enum::CombatantValue::TMelleeInitiative, value);
}
}

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

        if (nullptr != _combat_output)
        {
            for (const auto& item : side->GetCombatantList())
            {
                SetMelleeInititive(*item, *_random_sequence);
                _combat_output->CombatantAdded(*item, *side);
            }
        }
    }

    void RemoveSide(const int32_t side_id)
    {
        auto lambda = [side_id](const std::shared_ptr<static_lq::ICombatSide>& rhs) -> bool{ return rhs->GetId() == side_id;};
        const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);
        if (found != _side_array.end())
        {
            if (nullptr != _combat_output)
            {
                std::shared_ptr<static_lq::ICombatSide>& side = *found;
                for (const auto& item : side->GetCombatantList())
                {
                    _combat_output->CombatantRemoved(*item, *side);
                }
            }

            _side_array.erase(found);
        }
    }

    void AddCombatant(const std::shared_ptr<static_lq::ICombatant>& combatant, const int32_t side_id)
    {
        SetMelleeInititive(*combatant, *_random_sequence);
        auto lambda = [side_id](const std::shared_ptr<static_lq::ICombatSide>& rhs) { return rhs->GetId() == side_id;};
        const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);
        if (found != _side_array.end())
        {
            (*found)->AddCombatant(combatant);

            if (nullptr != _combat_output)
            {
                std::shared_ptr<static_lq::ICombatSide>& side = *found;
                _combat_output->CombatantAdded(*combatant, *side);
            }
        }
    }

    void RemoveCombatant(const int32_t combatant_id, const int32_t side_id)
    {
        auto lambda = [side_id](const std::shared_ptr<static_lq::ICombatSide>& rhs) { return rhs->GetId() == side_id;};
        const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);
        if (found != _side_array.end())
        {
            if (nullptr != _combat_output)
            {
                std::shared_ptr<static_lq::ICombatSide>& side = *found;
                for (const auto& item : side->GetCombatantList())
                {
                    if (item->GetId() == combatant_id)
                    {
                        _combat_output->CombatantRemoved(*item, *side);
                        break;
                    }
                }
            }

            (*found)->RemoveCombatant(combatant_id);
        }
    }

    void DoFirstSegmentOfTurn()
    {
        for (const auto& item: _side_array)
        {
            for (const auto& sub_item : item->GetCombatantList())
            {
                SetMelleeInititive(*sub_item, *_random_sequence);
            }
        }
    }

    void GatherMellee(std::vector<std::shared_ptr<static_lq::ICombatant>>& out_mellee, const static_lq::ICombatSide& in_side, const static_lq::ICombatant& in_subject)
    {
        for (const auto& item: in_side.GetCombatantList())
        {
            if (true == _combatant_topology->CanAttackMellee(in_subject, *item))
            {
                out_mellee.push_back(item);
            }
        }
    }

    void GatherRange(std::vector<std::shared_ptr<static_lq::ICombatant>>& out_range, const static_lq::ICombatSide& in_side, const static_lq::ICombatant& in_subject)
    {
        for (const auto& item: in_side.GetCombatantList())
        {
            if (true == _combatant_topology->CanAttackRange(in_subject, *item))
            {
                out_range.push_back(item);
            }
        }
    }

    void GatherActions(std::vector<std::shared_ptr<static_lq::ICombatAction>>& out_combat_action_array)
    {
        for (const auto& item: _side_array)
        {
            for (const auto& sub_item : item->GetCombatantList())
            {
                std::vector<std::shared_ptr<static_lq::ICombatant>> team_mellee;
                std::vector<std::shared_ptr<static_lq::ICombatant>> team_range;
                std::vector<std::shared_ptr<static_lq::ICombatant>> opponent_mellee;
                std::vector<std::shared_ptr<static_lq::ICombatant>> opponent_range;

                GatherMellee(team_mellee, *item, *sub_item);
                GatherRange(team_range, *item, *sub_item);
                for (const auto& other_side : _side_array)
                {
                    if (item->IsHostileToSide(*other_side))
                    {
                        GatherMellee(opponent_mellee, *other_side, *sub_item);
                        GatherRange(opponent_range, *other_side, *sub_item);
                    }
                }

                sub_item->GatherAction(
                    out_combat_action_array,
                    *_random_sequence,
                    _turn,
                    _segment,
                    team_mellee,
                    team_range,
                    opponent_mellee,
                    opponent_range
                    );
            }
        }
    }

    void PerformActions(const std::vector<std::shared_ptr<static_lq::ICombatAction>>& in_combat_action_array)
    {
        for (const auto& item: in_combat_action_array)
        {
            item->PerformAction(*_combat_output);
        }
    }

    // return true if combat can continue
    const bool AdvanceTime()
    {
        _segment += 1;
        while (10 < _segment)
        {
            _segment -= 10;
            _turn += 1;
        }

        _combat_output->SetTurnSegment(_turn, _segment);

        if (1 == _segment) 
        {
            DoFirstSegmentOfTurn();
        }

        std::vector<std::shared_ptr<static_lq::ICombatAction>> combat_action_array;
        GatherActions(combat_action_array);

        PerformActions(combat_action_array);

        std::vector<std::shared_ptr<static_lq::ICombatSide>> still_functional_sides = {};
        GatherStillFunctionalSides(still_functional_sides, _side_array);

        bool continue_combat = TestSidesAntagonistic(still_functional_sides);
        if (false == continue_combat)
        {
            _combat_output->SetCombatEnd(still_functional_sides);
        }
        return continue_combat;
    }

private:
    std::shared_ptr<static_lq::ICombatOutput> _combat_output;
    std::shared_ptr<static_lq::RandomSequence> _random_sequence;
    std::shared_ptr<static_lq::ICombatTopology> _combatant_topology;
    
    int32_t _turn = 0;
    int32_t _segment = 0;

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

void static_lq::Combat::RemoveSide(const int32_t side_id)
{
    _implementation->RemoveSide(side_id);
}

void static_lq::Combat::AddCombatant(const std::shared_ptr<ICombatant>& combatant, const int32_t side_id)
{
    _implementation->AddCombatant(combatant, side_id);
}

void static_lq::Combat::RemoveCombatant(const int32_t combatant_id, const int32_t side_id)
{
    _implementation->RemoveCombatant(combatant_id, side_id);
}

const bool static_lq::Combat::AdvanceTime()
{
    return _implementation->AdvanceTime();
}


