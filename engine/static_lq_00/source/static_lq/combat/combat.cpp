#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/combat.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/combat_time.h"
#include "static_lq/combat/i_combat_action.h"
#include "static_lq/combat/i_combat_output.h"
#include "static_lq/combat/i_combat_side.h"
#include "static_lq/combat/i_combat_topology.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/random_sequence.h"

namespace
{
void TriggerEffects(
	const std::vector<std::shared_ptr<StaticLq::ICombatSide>>& in_side_array,
	const StaticLq::CombatTime& in_combat_time,
	StaticLq::RandomSequence* in_random_sequence,
	StaticLq::ICombatOutput* in_output
	)
{
	if (nullptr == in_random_sequence)
	{
		return;
	}

	for (const auto& side : in_side_array)
	{
		for (const auto& player : side->GetCombatantList())
		{
			player->TriggerEffects(
				in_combat_time,
				*in_random_sequence,
				in_output
				);
		}
	}
}

void GatherStillFunctionalSides(std::vector<std::shared_ptr<StaticLq::ICombatSide>>& out_still_functional_sides, const std::vector<std::shared_ptr<StaticLq::ICombatSide>>& in_side_array, StaticLq::ICombatOutput* in_output)
{
	for (const auto& item : in_side_array)
	{
		if (true == item->CanContinueCombat())
		{
			in_output;
#if 0
			if (nullptr != in_output)
			{
				in_output->DebugLog(
					(item->GetDisplayName() + " still functional side\n").c_str()
 					);
				for (const auto& sub_item : item->GetCombatantList())
				{
					in_output->DebugLog(
						std::string(" ") + (sub_item->GetDisplayName() + " can still function:" + std::to_string(sub_item->GetValue(StaticLq::CombatEnum::CombatantValue::TCanContinueCombat)) + "\n").c_str()
						);
				}
			}
#endif //#if 0

			out_still_functional_sides.push_back(item);
		}
	}
}

const bool TestSidesAntagonistic(const std::vector<std::shared_ptr<StaticLq::ICombatSide>>& in_side_array)
{
	const int32_t count = static_cast<int32_t>(in_side_array.size());
	for (int32_t index = 0; index < count; ++index)
	{
		const StaticLq::ICombatSide* const side = in_side_array[index].get();
		for (int32_t sub_index = 0; sub_index < count; ++sub_index)
		{
			if (index == sub_index)
			{
				continue;
			}
			const StaticLq::ICombatSide* const other_side = in_side_array[sub_index].get();
			if (side->IsHostileToSide(*other_side))
			{
				return true;
			}
		}
	}
	return false;
}

// are there any post combat effect
const bool HasPostCombatEffect(const std::vector<std::shared_ptr<StaticLq::ICombatSide>>& in_side_array)
{
	for (const auto& side : in_side_array)
	{
		for (const auto& player : side->GetCombatantList())
		{
			if(player->HasPostCombatEffect())
			{
				return true;
			}
		}
	}
	return false;
}

void SetMelleeInititive(StaticLq::ICombatant& in_combatant, StaticLq::RandomSequence& in_random_sequence, StaticLq::ICombatOutput& combat_output)
{
	const int32_t value = in_random_sequence.GenerateDice(10);
	in_combatant.SetValue(StaticLq::CombatEnum::CombatantValue::TMelleeInitiative, value);
	combat_output.CombatantSetMelleeInitiative(in_combatant, value);
}
}

class CombatImplementation
{
public:
	CombatImplementation(
		const std::shared_ptr<StaticLq::ICombatOutput>& combat_output,
		const std::shared_ptr<StaticLq::RandomSequence>& random_sequence,
		const std::shared_ptr<StaticLq::ICombatTopology>& combatant_topology
		)
	: _combat_output(combat_output)
	, _random_sequence(random_sequence)
	, _combatant_topology(combatant_topology)
	{
		// nop
		_combat_output->SetCombatStart();
	}

	void AddSide(const std::shared_ptr<StaticLq::ICombatSide>& side)
	{
		// check for duplicates
		auto lambda = [side](const std::shared_ptr<StaticLq::ICombatSide>& rhs) { return rhs.get() == side.get();};
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
				SetMelleeInititive(*item, *_random_sequence, *_combat_output);
				_combat_output->CombatantAdded(*item, *side);
			}
		}
	}

	void RemoveSide(const int32_t side_id)
	{
		auto lambda = [side_id](const std::shared_ptr<StaticLq::ICombatSide>& rhs) -> bool{ return rhs->GetId() == side_id;};
		const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);
		if (found != _side_array.end())
		{
			if (nullptr != _combat_output)
			{
				std::shared_ptr<StaticLq::ICombatSide>& side = *found;
				for (const auto& item : side->GetCombatantList())
				{
					_combat_output->CombatantRemoved(*item, *side);
				}
			}

			_side_array.erase(found);
		}
	}

	void AddCombatant(const std::shared_ptr<StaticLq::ICombatant>& combatant, const int32_t side_id)
	{
		auto lambda = [side_id](const std::shared_ptr<StaticLq::ICombatSide>& rhs) { return rhs->GetId() == side_id;};
		const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);
		if (found != _side_array.end())
		{
			SetMelleeInititive(*combatant, *_random_sequence, *_combat_output);
			(*found)->AddCombatant(combatant);

			if (nullptr != _combat_output)
			{
				std::shared_ptr<StaticLq::ICombatSide>& side = *found;
				_combat_output->CombatantAdded(*combatant, *side);
			}
		}
	}

	void RemoveCombatant(const int32_t combatant_id, const int32_t side_id)
	{
		auto lambda = [side_id](const std::shared_ptr<StaticLq::ICombatSide>& rhs) { return rhs->GetId() == side_id;};
		const auto found = std::find_if(_side_array.begin(), _side_array.end(), lambda);
		if (found != _side_array.end())
		{
			if (nullptr != _combat_output)
			{
				std::shared_ptr<StaticLq::ICombatSide>& side = *found;
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
				SetMelleeInititive(*sub_item, *_random_sequence, *_combat_output);
			}
		}
	}

	void GatherMellee(std::vector<std::shared_ptr<StaticLq::ICombatant>>& out_mellee, const StaticLq::ICombatSide& in_side, const StaticLq::ICombatant& in_subject)
	{
		for (const auto& item: in_side.GetCombatantList())
		{
			if (true == _combatant_topology->CanAttackMellee(in_subject, *item))
			{
				out_mellee.push_back(item);
			}
		}
	}

	void GatherRange(std::vector<std::shared_ptr<StaticLq::ICombatant>>& out_range, const StaticLq::ICombatSide& in_side, const StaticLq::ICombatant& in_subject)
	{
		for (const auto& item: in_side.GetCombatantList())
		{
			if (true == _combatant_topology->CanAttackRange(in_subject, *item))
			{
				out_range.push_back(item);
			}
		}
	}

	void GatherActions(std::vector<std::shared_ptr<StaticLq::ICombatAction>>& out_combat_action_array)
	{
		for (const auto& item: _side_array)
		{
			for (const auto& sub_item : item->GetCombatantList())
			{
				std::vector<std::shared_ptr<StaticLq::ICombatant>> team_mellee;
				std::vector<std::shared_ptr<StaticLq::ICombatant>> team_range;
				std::vector<std::shared_ptr<StaticLq::ICombatant>> opponent_mellee;
				std::vector<std::shared_ptr<StaticLq::ICombatant>> opponent_range;

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
					_time,
					team_mellee,
					team_range,
					opponent_mellee,
					opponent_range
					);
			}
		}
	}

	void PerformActions(const std::vector<std::shared_ptr<StaticLq::ICombatAction>>& in_combat_action_array)
	{
		for (const auto& item: in_combat_action_array)
		{
			item->PerformAction(
				*_random_sequence,
				_time,
				_combat_output.get()
				);
		}
	}

	// return true if combat can continue
	const bool AdvanceTime()
	{
		_time.AdvanceTime();

		_combat_output->SetTurnSegment(_time.GetTurn(), _time.GetSegment());

		if (true == _time.IsStartOfTurn()) 
		{
			DoFirstSegmentOfTurn();
		}

		TriggerEffects(_side_array, _time, _random_sequence.get(), _combat_output.get());

		bool gather_action = false;
		{
			std::vector<std::shared_ptr<StaticLq::ICombatSide>> still_functional_sides = {};
			GatherStillFunctionalSides(still_functional_sides, _side_array, _combat_output.get());
			gather_action = TestSidesAntagonistic(still_functional_sides);
		}

		// gather the actions if there are sides wwhich can still function and opponenet sides that can continue combat
		if (true == gather_action)
		{
			std::vector<std::shared_ptr<StaticLq::ICombatAction>> combat_action_array;
			GatherActions(combat_action_array);

			PerformActions(combat_action_array);
		}

		// look for sides that can continue, or effects that have post combat effects (poison has a delay)
		{
			std::vector<std::shared_ptr<StaticLq::ICombatSide>> still_functional_sides = {};
			GatherStillFunctionalSides(still_functional_sides, _side_array, _combat_output.get());
			const bool still_have_antagonistic_sides = TestSidesAntagonistic(still_functional_sides);
			const bool has_post_combat_effect = HasPostCombatEffect(_side_array);
			const bool continue_combat = still_have_antagonistic_sides || has_post_combat_effect;

			if (false == continue_combat)
			{
				_combat_output->SetCombatEnd(still_functional_sides);
			}

			return continue_combat;
		}
	}

private:
	std::shared_ptr<StaticLq::ICombatOutput> _combat_output;
	std::shared_ptr<StaticLq::RandomSequence> _random_sequence;
	std::shared_ptr<StaticLq::ICombatTopology> _combatant_topology;
	
	StaticLq::CombatTime _time;

	std::vector<std::shared_ptr<StaticLq::ICombatSide>> _side_array;
};

StaticLq::Combat::Combat(
	const std::shared_ptr<ICombatOutput>& combat_output,
	const std::shared_ptr<RandomSequence>& random_sequence,
	const std::shared_ptr<ICombatTopology>& combatant_topology
	)
{
	_implementation = std::make_unique<CombatImplementation>(combat_output, random_sequence, combatant_topology);
}

StaticLq::Combat::~Combat()
{
	//nop
}

void StaticLq::Combat::AddSide(const std::shared_ptr<ICombatSide>& combat_side)
{
	_implementation->AddSide(combat_side);
}

void StaticLq::Combat::RemoveSide(const int32_t side_id)
{
	_implementation->RemoveSide(side_id);
}

void StaticLq::Combat::AddCombatant(const std::shared_ptr<ICombatant>& combatant, const int32_t side_id)
{
	_implementation->AddCombatant(combatant, side_id);
}

void StaticLq::Combat::RemoveCombatant(const int32_t combatant_id, const int32_t side_id)
{
	_implementation->RemoveCombatant(combatant_id, side_id);
}

const bool StaticLq::Combat::AdvanceTime()
{
	return _implementation->AdvanceTime();
}


