#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"

#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"
#include "common/locale/locale_system.h"
#include "common/tooltip/tooltip.h"
#include "common/tooltip/tooltip_data.h"
#include "static_lq/bestiary/bestiary.h"
#include "static_lq/combat/action/combat_action_mellee_attack.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/combat_time.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_action.h"
#include "static_lq/name/name_system.h"
#include "static_lq/random_sequence.h"

namespace
{
void AddDamage(DagThreadedCollection& in_dag_collection, const std::string& in_dag_key, const int32_t in_damage)
{
	auto node_key = in_dag_collection.FindNode(in_dag_key);
	const int32_t value = DagThreadedHelper::GetValue<int32_t>(in_dag_collection.GetDagValue(node_key));
	// in_damage can be positive or negative, but the total damage of a type, can not be negative
	const int32_t result = std::max(0, value + in_damage);

	in_dag_collection.SetDagValue(node_key, DagThreadedHelper::CreateDagValue<int32_t>(result));
}

}

StaticLq::SimpleCombatMonster::SimpleCombatMonster(
	const int32_t in_id,
	const std::shared_ptr<DagThreadedCollection>& in_dag_collection,
		const std::vector<MonsterAttackData> in_attack_data
	)
: _id(in_id)
, _dag_collection(in_dag_collection)
, _attack_data(in_attack_data)
{
}

const int StaticLq::SimpleCombatMonster::GetValue(const CombatEnum::CombatantValue in_key) 
{
	const std::string key = EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(in_key);
	const int32_t result = DagThreadedHelper::GetValue<int32_t>(_dag_collection->GetDagValue(_dag_collection->FindNode(key)));
	return result;
}

void StaticLq::SimpleCombatMonster::SetValue(const CombatEnum::CombatantValue in_key, const int32_t in_value) 
{
	const std::string key = EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(in_key);
	_dag_collection->SetDagValue(
		_dag_collection->FindNode(key), 
		DagThreadedHelper::CreateDagValue<int32_t>(in_value)
		);
	return;
}

std::shared_ptr<TooltipData> StaticLq::SimpleCombatMonster::GetTooltip(const CombatEnum::CombatantValue in_key, const LocaleSystem& in_locale_system) 
{
	const std::string key = EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(in_key);
	NodeID node_id = _dag_collection->FindNode(key);
	const std::string display_name = _dag_collection->GetDisplayName(node_id);
	const std::string tooltip = _dag_collection->GetTooltipRaw(node_id);

	std::shared_ptr<TooltipData> tooltip_data = std::make_shared<TooltipData>();

	tooltip_data->_text = in_locale_system.GetValue(display_name);
	tooltip_data->_link = DagThreaded::GetTooltipBody(*_dag_collection, node_id, in_locale_system);

	return tooltip_data;
}

void StaticLq::SimpleCombatMonster::GatherAction(
	std::vector<std::shared_ptr<ICombatAction>>& out_actions,
	RandomSequence& in_out_random_sequence,
	const CombatTime& in_combat_time,
	const std::vector<std::shared_ptr<ICombatant>>& in_team_mellee,
	const std::vector<std::shared_ptr<ICombatant>>& in_team_range,
	const std::vector<std::shared_ptr<ICombatant>>& in_opponent_mellee,
	const std::vector<std::shared_ptr<ICombatant>>& in_opponent_range
	) 
{
	out_actions;
	in_out_random_sequence;
	in_combat_time;
	in_team_mellee;
	in_team_range;
	in_opponent_mellee;
	in_opponent_range;

	if (in_combat_time.IsStartOfTurn())
	{
		_attack_index = 0;
		_mellee_attack_recovery = 0; // todo: we don't reset range recovery
	}


	if (0 != in_opponent_mellee.size())
	{
		std::shared_ptr<ICombatant> target = in_opponent_mellee[in_out_random_sequence.Generate() % in_opponent_mellee.size()];

		// can we do a melle attack
		bool can_mellee_attack = false;
		const int mellee_initiative = GetValue(StaticLq::CombatEnum::CombatantValue::TMelleeInitiative);
		if (mellee_initiative <= in_combat_time.GetSegment())
		{
			if (0 < _mellee_attack_recovery)
			{
				_mellee_attack_recovery -= 1;
			}
			else
			{
				const int opponent_mellee_initiative = target->GetValue(StaticLq::CombatEnum::CombatantValue::TMelleeInitiative);
				if ((_attack_index == 0) || (in_combat_time.GetSegment() < opponent_mellee_initiative))
				{
					can_mellee_attack = true;
				}
			}
		}

		if ((true == can_mellee_attack) && (0 < _attack_data.size()))
		{
			const MonsterAttackData& attack = _attack_data[_attack_index];
			_attack_index = (_attack_index + 1) % _attack_data.size();
			_mellee_attack_recovery = 0; //attack._recovery_time; 
			const int attack_roll = in_out_random_sequence.GenerateDice(30);
			const int32_t attack_bonus = GetValue(StaticLq::CombatEnum::CombatantValue::TAttackBonus);
			const int32_t defence = target->GetValue(StaticLq::CombatEnum::CombatantValue::TDefense);
			bool hit = false;
			int32_t severity = std::max(0, attack_roll + attack_bonus - defence);
			if ((30 == attack_roll) || (29 == attack_roll) || (28 == attack_roll))
			{
				hit = true;
			}
			else if ((1 == attack_roll) || (2 == attack_roll) || (3 == attack_roll))
			{
				// nop
			}
			else  if (defence < attack_roll + attack_bonus)
			{
				hit = true;
			}

			if (true == hit)
			{
				int32_t pysical_damage = attack._damage._constant;
				for (int32_t index = 0; index < attack._damage._dice_count; ++index)
				{
					pysical_damage += in_out_random_sequence.GenerateDice(attack._damage._dice_base);
				}
				if ((true == attack._cause_severity_damage) && (0 != target->GetValue(StaticLq::CombatEnum::CombatantValue::TSusceptibleSeverityDamage)))
				{
					pysical_damage += severity;
				}

				ICombatant* combatant_performing_action = this;
				std::shared_ptr<CombatActionMelleeAttack> action = std::make_shared<CombatActionMelleeAttack>(
					combatant_performing_action, 
					target.get(), 
					pysical_damage
					);
				std::shared_ptr<ICombatAction> action_down = action;
				out_actions.push_back(action_down);
			}
		}
	}
}

// you could use SetValue, but there is some logic to not allow negative value totals for damage.
// positive to do damage, negative to heal
void StaticLq::SimpleCombatMonster::ApplyDamageDelta(
	const int32_t in_physical_damage_delta,
	const int32_t in_fatigue_damage_delta,
	const int32_t in_paralyzation_damage_delta
	)
{
	AddDamage(*_dag_collection, 
			EnumSoftBind<StaticLq::BestiaryEnum::CombatantValueInternal>::EnumToString(StaticLq::BestiaryEnum::CombatantValueInternal::TDamagePhysical),
			in_physical_damage_delta);
	AddDamage(*_dag_collection, 
			EnumSoftBind<StaticLq::BestiaryEnum::CombatantValueInternal>::EnumToString(StaticLq::BestiaryEnum::CombatantValueInternal::TDamageFatigue),
			in_fatigue_damage_delta);
	AddDamage(*_dag_collection, 
			EnumSoftBind<StaticLq::BestiaryEnum::CombatantValueInternal>::EnumToString(StaticLq::BestiaryEnum::CombatantValueInternal::TDamageParalyzation),
			in_paralyzation_damage_delta);
}
