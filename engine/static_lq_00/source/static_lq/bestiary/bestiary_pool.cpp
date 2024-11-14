#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/bestiary_pool.h"

#include "common/locale/locale_enum.h"
#include "common/locale/locale_system.h"
#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"
#include "static_lq/bestiary/monster_data.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"
#include "static_lq/name/name_system.h"
#include "static_lq/random_sequence.h"

namespace
{
	constexpr char s_locale_key_value[] = "slqsc_value";
	constexpr char s_locale_key_self[] = "slqsc_self";
	constexpr char s_locale_key_self_tooltip[] = "slqsc_self_tooltip";
	constexpr char s_locale_key_species_giant_ant[] = "slqsc_species_giant_ant";
	constexpr char s_locale_key_description_giant_ant[] = "slqsc_description_giant_ant";
	constexpr char s_locale_key_attack_mandibles[] = "slqsc_bestiary_attack_mandibles";

	constexpr char s_locale_key_species_giant_spider[] = "slqsc_species_giant_spider";
	constexpr char s_locale_key_description_giant_spider[] = "slqsc_description_giant_spider";
	constexpr char s_locale_key_attack_bite[] = "slqsc_bestiary_attack_bite";
	constexpr char s_locale_key_damage_tolerance[] = "slqsc_damage_tolerance";
	constexpr char s_locale_key_damage_tolerance_tooltip[] = "slqsc_damage_tolerance_tooltip";
	constexpr char s_locale_key_damage_sum[] = "slqsc_damage_sum";
	constexpr char s_locale_key_damage_paralyzation_sum[] = "slqsc_damage_sum";
	constexpr char s_locale_key_sum_tooltip[] = "slqsc_damage_sum_tooltip";
	constexpr char s_locale_key_fatigue_damage[] = "slqsc_fatigue_damage";
	constexpr char s_locale_key_physical_damage[] = "slqsc_physical_damage";
	constexpr char s_locale_key_paralyzation_damage[] = "slqsc_paralyzation_damage";
	constexpr char s_locale_key_alive[] = "slqsc_alive";
	constexpr char s_locale_key_paralyzed[] = "slqsc_paralyzed";	
	constexpr char s_locale_key_0_less_i0_minus_i1[] = "slqsc_alive_tooltip";
	constexpr char s_locale_key_can_continue_combat[] = "slqsc_can_continue_combat";
	constexpr char s_locale_key_can_continue_combat_tooltip[] = "slqsc_can_continue_combat_tooltip";
	constexpr char s_locale_key_stack_sum_tooltip[] = "slqsc_stack_sum";

	constexpr char s_locale_key_physical_strength[] = "slqsc_physical_strength";
	constexpr char s_locale_key_stamina[] = "slqsc_stamina";
	constexpr char s_locale_key_agility[] = "slqsc_agility";
	constexpr char s_locale_key_manual_dexterity[] = "slqsc_manual_dexterity";
	constexpr char s_locale_key_perception[] = "slqsc_perception";
	constexpr char s_locale_key_willpower[] = "slqsc_willpower";
	constexpr char s_locale_key_faith[] = "slqsc_faith";

	constexpr char s_key_giant_ant[] = "giant_ant";
	constexpr char s_key_giant_ant_worker[] = "giant_ant_worker";
	constexpr char s_key_giant_ant_warrior[] = "giant_ant_warrior";
	constexpr char s_key_giant_ant_queen[] = "giant_ant_queen";
	constexpr char s_key_giant_ant_dew_pot_worker[] = "giant_ant_dew_pot_worker";
	constexpr char s_key_giant_spider_average[] = "giant_spider_average";
	constexpr char s_key_giant_spider_below_average[] = "giant_spider_below_average";
	constexpr char s_key_giant_spider_above_average[] = "giant_spider_above_average";
	constexpr char s_key_giant_spider_exceptional[] = "giant_spider_exceptional";

	template<typename TYPE> const TYPE& GetRef(const std::shared_ptr<StaticLq::MonsterData>& in_monster_data, const StaticLq::BestiaryEnum::MonsterAttribute in_attribute)
	{
		if (nullptr != in_monster_data)
		{
			auto found = in_monster_data->MapAttibute.find(in_attribute);
			if (found != in_monster_data->MapAttibute.end())
			{
				if (std::holds_alternative<TYPE>(found->second))
				{
					return std::get<TYPE>(found->second);
				}
			}
		}
		static const TYPE result = {};
		return result;
	}

	template<typename TYPE> const TYPE GetValue(const std::shared_ptr<StaticLq::MonsterData>& in_monster_data, const StaticLq::BestiaryEnum::MonsterAttribute in_attribute)
	{
		if (nullptr != in_monster_data)
		{
			auto found = in_monster_data->MapAttibute.find(in_attribute);
			if (found != in_monster_data->MapAttibute.end())
			{
				if (std::holds_alternative<TYPE>(found->second))
				{
					return std::get<TYPE>(found->second);
				}
			}
		}
		return static_cast<TYPE>(0);
	}

	void DagAddName(DagThreadedCollection& in_dag_collection, const std::string& in_name_key, const std::string& in_species, const std::string& in_variation)
	{
		auto name_key = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDisplayName),
			DagThreadedHelper::CreateDagValue<std::string>(in_name_key),
			DagThreaded::ValueChanged,
			s_locale_key_value
			);
		auto species = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSpecies),
			DagThreadedHelper::CreateDagValue<std::string>(in_species),
			DagThreaded::ValueChanged,
			s_locale_key_value
			);
		auto variation = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TVariation),
			DagThreadedHelper::CreateDagValue<std::string>(in_variation),
			DagThreaded::ValueChanged,
			s_locale_key_value
			);

		// or a calculate node so that the tooltip changes on input change
		auto self = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSelf),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >&
			) -> std::shared_ptr< IDagThreadedValue >
			{
				return DagThreadedHelper::CreateDagValue<std::string>("");
			},
			s_locale_key_self,
			s_locale_key_self_tooltip
			);
		in_dag_collection.AddNodeLinkStack(self, name_key);
		in_dag_collection.AddNodeLinkStack(self, species);
		in_dag_collection.AddNodeLinkStack(self, variation);
	}

	void DagAddDamageTolerance(DagThreadedCollection& in_dag_collection, const int32_t in_id, const StaticLq::RollData& in_roll_data)
	{
		auto damage_tolerance_seed = in_dag_collection.CreateNodeVariable(
			in_dag_collection.MakeUid(), 
			DagThreadedHelper::CreateDagValue<int32_t>(in_id)
			);
		auto damage_tolerance_constant = in_dag_collection.CreateNodeVariable(
		EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTolleranceConstant),
			DagThreadedHelper::CreateDagValue<int32_t>(in_roll_data._constant)
			);
		auto damage_tolerance_dice_count = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTolleranceDiceCount),
			DagThreadedHelper::CreateDagValue<int32_t>(in_roll_data._dice_count)
			);
		auto damage_tolerance_dice_base = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTolleranceDiceSide),
			DagThreadedHelper::CreateDagValue<int32_t>(in_roll_data._dice_base)
			);

		auto damage_tolerance = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTollerance),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
			) -> std::shared_ptr< IDagThreadedValue >
			{
				const int32_t id = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
				const int32_t constant = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
				const int32_t dice_count = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[2]);
				const int32_t dice_base = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[3]);
				int32_t result = constant;
				StaticLq::RandomSequence random_sequence(id);
				for (int index = 0; index < dice_count; ++index)
				{
					result += random_sequence.GenerateDice(dice_base);
				}

				return DagThreadedHelper::CreateDagValue<int32_t>(result);
			},
			s_locale_key_damage_tolerance,
			s_locale_key_damage_tolerance_tooltip
			);

		in_dag_collection.AddNodeLinkIndexed(damage_tolerance, damage_tolerance_seed, 0);
		in_dag_collection.AddNodeLinkIndexed(damage_tolerance, damage_tolerance_constant, 1);
		in_dag_collection.AddNodeLinkIndexed(damage_tolerance, damage_tolerance_dice_count, 2);
		in_dag_collection.AddNodeLinkIndexed(damage_tolerance, damage_tolerance_dice_base, 3);
	}

	void DagAddDamageSum(DagThreadedCollection& in_dag_collection)
	{
		auto fatigue_damage = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageFatigue),
			DagThreadedHelper::CreateDagValue<int32_t>(0),
			DagThreaded::ValueChanged,
			s_locale_key_fatigue_damage
			);
		auto physical_damage = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamagePhysical),
			DagThreadedHelper::CreateDagValue<int32_t>(0),
			DagThreaded::ValueChanged,
			s_locale_key_physical_damage
			);
		auto paralyzation_damage = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageParalyzation),
			DagThreadedHelper::CreateDagValue<int32_t>(0),
			DagThreaded::ValueChanged,
			s_locale_key_paralyzation_damage
			);

		auto damage_sum = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageSum),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
			) -> std::shared_ptr< IDagThreadedValue >
			{
				const int32_t fatigue_damage_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
				const int32_t physical_damage_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
				//const int32_t paralyzation_damage_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[2]);
				const int32_t sum = fatigue_damage_value + physical_damage_value; // + paralyzation_damage_value;
				return DagThreadedHelper::CreateDagValue<int32_t>(sum);
			},
			s_locale_key_damage_sum,
			s_locale_key_sum_tooltip
			);

		in_dag_collection.AddNodeLinkIndexed(damage_sum, fatigue_damage, 0);
		in_dag_collection.AddNodeLinkIndexed(damage_sum, physical_damage, 1);
		//in_dag_collection.AddNodeLinkIndexed(damage_sum, paralyzation_damage, 2);

		auto damage_paralyzation_sum = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageParalyzationSum),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
			) -> std::shared_ptr< IDagThreadedValue >
			{
				const int32_t damage_sum = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
				const int32_t paralyzation_damage = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
				const int32_t sum = damage_sum + paralyzation_damage;
				return DagThreadedHelper::CreateDagValue<int32_t>(sum);
			},
			s_locale_key_damage_paralyzation_sum,
			s_locale_key_sum_tooltip
			);

		in_dag_collection.AddNodeLinkIndexed(damage_paralyzation_sum, damage_sum, 0);
		in_dag_collection.AddNodeLinkIndexed(damage_paralyzation_sum, paralyzation_damage, 1);
	}

	void DagAddStatus(DagThreadedCollection& in_dag_collection)
	{
		auto alive = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TIsAlive),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
			) -> std::shared_ptr< IDagThreadedValue >
			{
				const int32_t damage_tolerance_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
				const int32_t damage_sum_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
				const int32_t alive = 0 < (damage_tolerance_value - damage_sum_value);
				return DagThreadedHelper::CreateDagValue<int32_t>(alive);
			},
			s_locale_key_alive,
			s_locale_key_0_less_i0_minus_i1
			);

		in_dag_collection.AddNodeLinkIndexed(
			alive, 
			in_dag_collection.FindNode(EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTollerance)),
			0);
		in_dag_collection.AddNodeLinkIndexed(
			alive, 
			in_dag_collection.FindNode(EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageSum)),
			1);

		auto paralyzed = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TIsParalyzed),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
			) -> std::shared_ptr< IDagThreadedValue >
			{
				const int32_t damage_paralyzation_sum_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
				const int32_t damage_tolerance_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
				const int32_t paralyzed = 0 < (damage_paralyzation_sum_value - damage_tolerance_value);
				return DagThreadedHelper::CreateDagValue<int32_t>(paralyzed);
			},
			s_locale_key_paralyzed,
			s_locale_key_0_less_i0_minus_i1
			);

		in_dag_collection.AddNodeLinkIndexed(
			paralyzed, 
			in_dag_collection.FindNode(EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageParalyzationSum)),
			0);
		in_dag_collection.AddNodeLinkIndexed(
			paralyzed, 
			in_dag_collection.FindNode(EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTollerance)),
			1);

	}

	void DagAddCanContinueCombat(DagThreadedCollection& in_dag_collection)
	{
		auto can_continue_combat = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TCanContinueCombat),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
			) -> std::shared_ptr< IDagThreadedValue >
			{
				const int32_t alive = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
				const int32_t paralyzed = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
				const int32_t can_continue = (1 == alive) && (0 == paralyzed);
				return DagThreadedHelper::CreateDagValue<int32_t>(can_continue);
			},
			s_locale_key_can_continue_combat,
			s_locale_key_can_continue_combat_tooltip
			);
		in_dag_collection.AddNodeLinkIndexed(
			can_continue_combat, 
			in_dag_collection.FindNode(EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TIsAlive)),
			0);
		in_dag_collection.AddNodeLinkIndexed(
			can_continue_combat, 
			in_dag_collection.FindNode(EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TIsParalyzed)),
			1);
	}

	void DagAddCombatVariables(DagThreadedCollection& in_dag_collection, const std::shared_ptr<StaticLq::MonsterData>& in_monster_data)
	{
		auto health_points = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::THealthPoints),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
			) -> std::shared_ptr< IDagThreadedValue >
			{
				const int32_t a = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
				const int32_t b = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
				const int32_t result = a - b;
				return DagThreadedHelper::CreateDagValue<int32_t>(result);
			}
			);

		in_dag_collection.AddNodeLinkIndexed(
			health_points, 
			in_dag_collection.FindNode(EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTollerance)),
			0);
		in_dag_collection.AddNodeLinkIndexed(
			health_points, 
			in_dag_collection.FindNode(EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageParalyzationSum)),
			1);

		// we need a node to hold the value
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TMelleeInitiative),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);

		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TCombatLevel),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TCombatLevel))
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TExperencePointValue),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TExperencePointValue))
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TAttackBonus),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAttackBonus))
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDefense),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TDefence))
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TRecoveryTime),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TRecoveryTime))
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptibleSeverityDamage),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TIsEffectedBySeverityDamage))
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptiblePoison),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TIsEffectedByPoison))
			);
		//TSusceptibleFaithDamage, // combat
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptibleFaithDamage),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TIsEffectedByFaithDamage))
			);
		//TSusceptibleNonMagicalDamage, // combat
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptibleNonMagicalDamage),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TIsEffectedByPysicalDamage))
			);
		//TSusceptibleMagicalDamage, // combat
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptibleMagicalDamage),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TIsEffectedByMagicDamage))
			);

		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TAbsorption),
			DagThreadedHelper::CreateDagValue<int32_t>(GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAbsorption))
			);

		//TAlignment
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TAlignment),
			DagThreadedHelper::CreateDagValue<int32_t>(static_cast<int32_t>(GetValue<StaticLq::BestiaryEnum::Alignment>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAlignment)))
			);

		//TPoisonDoseCountToxin, 
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountToxin),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);

		//TPoisonDoseCountHallucinogen,
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountHallucinogen),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);
		//TPoisonDoseCountVenom,
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountVenom),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);

		//TPoisonDoseCountKillingVenom,
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountKillingVenom),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);

		//TPoisonDoseCountParalyzingVenom,
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountParalyzingVenom),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);

		//TPoisonSaveFailedToxin, 
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedToxin),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);
		//TPoisonSaveFailedHallucinogen,
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedHallucinogen),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);

		//TPoisonSaveFailedVenom,
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedVenom),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);

		//TPoisonSaveFailedKillingVenom,
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedKillingVenom),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);

		//TPoisonSaveFailedParalyzingVenom,
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedParalyzingVenom),
			DagThreadedHelper::CreateDagValue<int32_t>(0)
			);

		return;
	}

	void AddAttribute(DagThreadedCollection& in_dag_collection, const std::optional<int32_t>& in_value, const StaticLq::CombatEnum::CombatantValue in_raw_enum, const StaticLq::CombatEnum::CombatantValue in_buff_enum, const StaticLq::CombatEnum::CombatantValue in_output_enum, const std::string& in_attribute_locale_key)
	{
		// todo: do we need a "is valid" flag
		const int32_t value = in_value.has_value() ? in_value.value() : 0;

		auto raw = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(in_raw_enum),
			DagThreadedHelper::CreateDagValue<int32_t>(value)
			);

		auto buff = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(in_buff_enum),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_stack, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >&
			) -> std::shared_ptr< IDagThreadedValue >
			{
				int32_t result = 0;
				for (const auto& item : in_array_stack)
				{
					result += DagThreadedHelper::GetValue<int32_t>(item);
				}
				return DagThreadedHelper::CreateDagValue<int32_t>(result);
			},
			"",
			s_locale_key_stack_sum_tooltip
			);

		auto output_node = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(in_output_enum),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
			) -> std::shared_ptr< IDagThreadedValue >
			{
				const int32_t zero = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
				const int32_t one = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
				const int32_t sum = zero + one;
				return DagThreadedHelper::CreateDagValue<int32_t>(sum);
			},
			in_attribute_locale_key,
			s_locale_key_sum_tooltip
			);

		in_dag_collection.AddNodeLinkIndexed(output_node, raw, 0);
		in_dag_collection.AddNodeLinkIndexed(output_node, buff, 1);

	}

	void DagAddAttributes(DagThreadedCollection& in_dag_collection, const std::shared_ptr<StaticLq::MonsterData>& in_monster_data)
	{
		AddAttribute(in_dag_collection, GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAttributeAgility),
			StaticLq::CombatEnum::CombatantValue::TRawAgility, StaticLq::CombatEnum::CombatantValue::TBuffAgility, StaticLq::CombatEnum::CombatantValue::TAgility, s_locale_key_agility);
		AddAttribute(in_dag_collection, GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAttributeFaith),
			StaticLq::CombatEnum::CombatantValue::TRawFaith, StaticLq::CombatEnum::CombatantValue::TBuffFaith, StaticLq::CombatEnum::CombatantValue::TFaith, s_locale_key_faith);
		AddAttribute(in_dag_collection, GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAttributeManualDexterity),
			StaticLq::CombatEnum::CombatantValue::TRawManualDexterity, StaticLq::CombatEnum::CombatantValue::TBuffManualDexterity, StaticLq::CombatEnum::CombatantValue::TManualDexterity, s_locale_key_manual_dexterity);
		AddAttribute(in_dag_collection, GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAttributePerception),
			StaticLq::CombatEnum::CombatantValue::TRawPerception, StaticLq::CombatEnum::CombatantValue::TBuffPerception, StaticLq::CombatEnum::CombatantValue::TPerception, s_locale_key_perception);
		AddAttribute(in_dag_collection, GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAttributePhysicalStrength),
			StaticLq::CombatEnum::CombatantValue::TRawPhysicalStrength, StaticLq::CombatEnum::CombatantValue::TBuffPhysicalStrength, StaticLq::CombatEnum::CombatantValue::TPhysicalStrength, s_locale_key_physical_strength);
		AddAttribute(in_dag_collection, GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAttributeStamina),
			StaticLq::CombatEnum::CombatantValue::TRawStamina, StaticLq::CombatEnum::CombatantValue::TBuffStamina, StaticLq::CombatEnum::CombatantValue::TStamina, s_locale_key_stamina);
		AddAttribute(in_dag_collection, GetValue<int32_t>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TAttributeWillpower),
			StaticLq::CombatEnum::CombatantValue::TRawWillpower, StaticLq::CombatEnum::CombatantValue::TBuffWillpower, StaticLq::CombatEnum::CombatantValue::TWillpower, s_locale_key_willpower);
	}


	std::shared_ptr<DagThreadedCollection> MakeMonsterDag(const int32_t in_id, const std::shared_ptr<StaticLq::MonsterData>& in_monster_data, const std::string& in_name_key)
	{
		std::shared_ptr<DagThreadedCollection> result = DagThreadedCollection::Factory();

		DagAddName(*result, in_name_key, GetRef<std::string>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TSpeciesName), GetRef<std::string>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TVariationName));
		DagAddDamageTolerance(*result, in_id, GetRef<StaticLq::RollData>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TDamageToleranceRollData));
		DagAddDamageSum(*result);
		DagAddStatus(*result);
		DagAddCanContinueCombat(*result);
		DagAddCombatVariables(*result, in_monster_data);
		DagAddAttributes(*result, in_monster_data);

		return result;
	}
}

class BestiaryPoolImplementation
{
public:
	BestiaryPoolImplementation()
	{
		// nop
	}

	void AddData(const std::string& in_key, const std::shared_ptr<StaticLq::MonsterData>& in_data)
	{
		_map_data[in_key] = in_data;
		// clear the flattened data on new data being added
		_map_data_flattened.clear();
	}

	std::shared_ptr<StaticLq::ICombatant> MakeCombatant(
		const std::string& in_key,
		StaticLq::NameSystem& in_name_system, 
		LocaleSystem& in_locale_system 
		)
	{
		const auto monster_data = GetMonsterData(in_key);
		const int id = StaticLq::ICombatant::MakeNewId();
		const std::string name_key = in_name_system.GenerateName(
			GetRef<std::string>(monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TNameKey),
			id, 
			in_locale_system
			);
		const int32_t variation_index = 1;
		auto dag_collection = MakeMonsterDag(id, monster_data, name_key);
		auto result = std::make_shared<StaticLq::SimpleCombatMonster>(
			id, 
			dag_collection, 
			GetRef<std::vector<StaticLq::MonsterAttackData>>(monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TMonsterAttackData)
			);
		return result;
	}

private:
	const std::shared_ptr<StaticLq::MonsterData> GetMonsterData(const std::string& in_key)
	{
		if (true == in_key.empty())
		{
			return std::make_shared<StaticLq::MonsterData>();
		}

		auto found_flattened = _map_data_flattened.find(in_key);
		if (found_flattened != _map_data_flattened.end())
		{
			return found_flattened->second;
		}

		std::shared_ptr<StaticLq::MonsterData> result = {};
		auto found = _map_data.find(in_key);
		if (found != _map_data.end())
		{
			//const StaticLq::MonsterData& source = *found->second;
			const std::string& parent_id = found->second->ParentID;
			if (false == parent_id.empty())
			{
				auto parent = GetMonsterData(parent_id);
				result = StaticLq::MonsterData::FactoryOverlay(parent, found->second);
			}
			else
			{
				result = found->second;
			}
		}

		_map_data_flattened[in_key] = result;

		return result;
	}
	
private:
	std::map<std::string, std::shared_ptr<StaticLq::MonsterData>> _map_data = {};
	std::map<std::string, std::shared_ptr<StaticLq::MonsterData>> _map_data_flattened = {};
};

void StaticLq::BestiaryPool::RegisterLocaleSystem(LocaleSystem& in_out_locale_system)
{
	const std::vector<LocaleSystem::Data> data = {
		{s_locale_key_value, "{_value}"},
		{s_locale_key_self, "{DisplayName}"},
		{s_locale_key_self_tooltip, "{DisplayName} the {Species}, {Variation}"},
		{s_locale_key_damage_tolerance, "Damage Tolerance ({_value})"},
		{s_locale_key_damage_tolerance_tooltip, "{_text} = {_i.1} + {_i.2}d{_i.3}"},

		{s_locale_key_species_giant_ant, "Giant Ant"},
		{s_locale_key_description_giant_ant, "giant ant description text"},
		{s_locale_key_attack_mandibles, "mandibles"},

		{s_locale_key_species_giant_spider, "Giant Spider"},
		{s_locale_key_description_giant_spider, "giant spider description text"},
		{s_locale_key_attack_bite, "bite"},

		{s_key_giant_ant_worker, "worker"},
		{s_key_giant_ant_warrior, "warrior"},
		{s_key_giant_ant_queen, "giant_ant_queen"},
		{s_key_giant_ant_dew_pot_worker, "giant_ant_dew_pot_worker"},
		{s_key_giant_spider_average, "giant_spider_average"},
		{s_key_giant_spider_below_average, "giant_spider_below_average"},
		{s_key_giant_spider_above_average, "giant_spider_above_average"},
		{s_key_giant_spider_exceptional, "giant_spider_exceptional"},

		};

	in_out_locale_system.Append(LocaleISO_639_1::Default, data);
}

std::shared_ptr<StaticLq::BestiaryPool> StaticLq::BestiaryPool::FactoryWithTestData()
{
	std::shared_ptr<BestiaryPool> result = std::make_shared<BestiaryPool>();

	//tome_of_terrors.pdf page:32
	// s_key_giant_ant
	{
		std::shared_ptr<MonsterData> monster_data = std::make_shared<MonsterData>();
		monster_data->MapAttibute = {
			//TVariationName, // "average", "warrior", "queen", "worker"
			{ BestiaryEnum::MonsterAttribute::TNameKey, MonsterData::TVariant(NameSystem::GetKeyGiantAnt())},
			{ BestiaryEnum::MonsterAttribute::TSpeciesName, MonsterData::TVariant(std::string(s_locale_key_species_giant_ant)) },
			{ BestiaryEnum::MonsterAttribute::TDescription, MonsterData::TVariant(std::string(s_locale_key_description_giant_ant)) },

			//TCombatLevel,
			//TAttackBonus,
			//TDefence,
			//TDamageToleranceRollData,
			//TExperencePointValue,

			{ BestiaryEnum::MonsterAttribute::TSpeed, MonsterData::TVariant(int32_t(70)) },
			{ BestiaryEnum::MonsterAttribute::TRecoveryTime, MonsterData::TVariant(int32_t(7)) },
			{ BestiaryEnum::MonsterAttribute::TAbsorption, MonsterData::TVariant(int32_t(6)) },
			{ BestiaryEnum::MonsterAttribute::TSurpriseAdjust, MonsterData::TVariant(int32_t(0)) },

			{ BestiaryEnum::MonsterAttribute::TMonsterType, MonsterData::TVariant(BestiaryEnum::MonsterType::LivingCreature) },
			{ BestiaryEnum::MonsterAttribute::THabitat, MonsterData::TVariant(BestiaryEnum::Habitat::TEverywhere) },
			{ BestiaryEnum::MonsterAttribute::TLifestyle, MonsterData::TVariant(
				static_cast<BestiaryEnum::Lifestyle>(
					static_cast<int32_t>(BestiaryEnum::Lifestyle::TInstinctive) | 
					static_cast<int32_t>(BestiaryEnum::Lifestyle::TComunal) | 
					static_cast<int32_t>(BestiaryEnum::Lifestyle::TInsect) 
				)) },
			{ BestiaryEnum::MonsterAttribute::TWealthType, MonsterData::TVariant(BestiaryEnum::WealthType::TMineral) },
			{ BestiaryEnum::MonsterAttribute::TAlignment, MonsterData::TVariant(BestiaryEnum::Alignment::TNeutral) },
			{ BestiaryEnum::MonsterAttribute::TCunning, MonsterData::TVariant(BestiaryEnum::Cunning::TLow) },
			{ BestiaryEnum::MonsterAttribute::TStrength, MonsterData::TVariant(BestiaryEnum::Strength::THigh) },
			{ BestiaryEnum::MonsterAttribute::TSize, MonsterData::TVariant(
				static_cast<BestiaryEnum::Size>(
					static_cast<int32_t>(BestiaryEnum::Size::TSmall) | 
					static_cast<int32_t>(BestiaryEnum::Size::TMedium) 
				))},
			//TSpecialCharacteristicData,

			{ BestiaryEnum::MonsterAttribute::TMonsterAttackData, MonsterData::TVariant(
				std::vector<MonsterAttackData>({
					{
						std::string(s_locale_key_attack_mandibles),
						RollData({0, 2, 4}),
						true
					}
				}))},

			{ BestiaryEnum::MonsterAttribute::TAttributePhysicalStrength, MonsterData::TVariant(int32_t(6)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeStamina, MonsterData::TVariant(int32_t(6)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeAgility, MonsterData::TVariant(int32_t(0)) },
			//TAttributeManualDexterity,
			{ BestiaryEnum::MonsterAttribute::TAttributePerception, MonsterData::TVariant(int32_t(0)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeWillpower, MonsterData::TVariant(int32_t(-3)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeFaith, MonsterData::TVariant(int32_t(-3)) },

			{ BestiaryEnum::MonsterAttribute::TIsEffectedByPoison, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedBySeverityDamage, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedByPysicalDamage, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedByMagicDamage, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedByFaithDamage, MonsterData::TVariant(int32_t(0)) },
		};

		result->_implementation->AddData(s_key_giant_ant, monster_data);
	}

	// s_key_giant_ant_worker
	{
		std::shared_ptr<MonsterData> monster_data = std::make_shared<MonsterData>();
		monster_data->ParentID = s_key_giant_ant;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, MonsterData::TVariant(std::string(s_key_giant_ant_worker))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, MonsterData::TVariant(int32_t(1))},
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, MonsterData::TVariant(int32_t(3))},
			{ BestiaryEnum::MonsterAttribute::TDefence, MonsterData::TVariant(int32_t(21))},
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, MonsterData::TVariant(RollData({15, 1, 10}))},
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, MonsterData::TVariant(int32_t(15))},
		};

		result->_implementation->AddData(s_key_giant_ant_worker, monster_data);
	}

	// s_key_giant_ant_warrior
	{
		std::shared_ptr<MonsterData> monster_data = std::make_shared<MonsterData>();
		monster_data->ParentID = s_key_giant_ant;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, MonsterData::TVariant(std::string(s_key_giant_ant_warrior))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, MonsterData::TVariant(int32_t(4))},
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, MonsterData::TVariant(int32_t(6))},
			{ BestiaryEnum::MonsterAttribute::TDefence, MonsterData::TVariant(int32_t(22))},
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, MonsterData::TVariant(RollData({15, 4, 10}))},
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, MonsterData::TVariant(int32_t(35))},

			{ BestiaryEnum::MonsterAttribute::TMonsterAttackData, MonsterData::TVariant(
				std::vector<MonsterAttackData>({
					{
						std::string(s_locale_key_attack_mandibles),
						RollData({0, 5, 4}),
						true
					}
				}))},
		};

		result->_implementation->AddData(s_key_giant_ant_warrior, monster_data);
	}

	// s_key_giant_ant_dew_pot_worker
	{
		std::shared_ptr<MonsterData> monster_data = std::make_shared<MonsterData>();
		monster_data->ParentID = s_key_giant_ant;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, MonsterData::TVariant(std::string(s_key_giant_ant_dew_pot_worker))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, MonsterData::TVariant(int32_t(0))},
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, MonsterData::TVariant(int32_t(0))},
			{ BestiaryEnum::MonsterAttribute::TDefence, MonsterData::TVariant(int32_t(5))},
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, MonsterData::TVariant(RollData({15, 1, 10}))},
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, MonsterData::TVariant(int32_t(1))},
		};

		result->_implementation->AddData(s_key_giant_ant_dew_pot_worker, monster_data);
	}

	// s_key_giant_ant_queen
	{
		std::shared_ptr<MonsterData> monster_data = std::make_shared<MonsterData>();
		monster_data->ParentID = s_key_giant_ant;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, MonsterData::TVariant(std::string(s_key_giant_ant_queen))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, MonsterData::TVariant(int32_t(0))},
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, MonsterData::TVariant(int32_t(0))},
			{ BestiaryEnum::MonsterAttribute::TDefence, MonsterData::TVariant(int32_t(5))},
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, MonsterData::TVariant(RollData({80, 2, 10}))},
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, MonsterData::TVariant(int32_t(2))},
		};

		result->_implementation->AddData(s_key_giant_ant_queen, monster_data);
	}

	//tome_of_terrors.pdf page:142
	// s_key_giant_spider_average
	{
		std::shared_ptr<MonsterData> monster_data = std::make_shared<MonsterData>();
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, MonsterData::TVariant(std::string(s_key_giant_spider_average))},

			{ BestiaryEnum::MonsterAttribute::TNameKey, MonsterData::TVariant(NameSystem::GetKeyGiantSpider())},
			{ BestiaryEnum::MonsterAttribute::TSpeciesName, MonsterData::TVariant(std::string(s_locale_key_species_giant_spider)) },
			{ BestiaryEnum::MonsterAttribute::TDescription, MonsterData::TVariant(std::string(s_locale_key_description_giant_spider)) },

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, MonsterData::TVariant(int32_t(3)) },
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, MonsterData::TVariant(int32_t(5)) },
			{ BestiaryEnum::MonsterAttribute::TDefence, MonsterData::TVariant(int32_t(24)) },
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, MonsterData::TVariant(RollData({10, 3, 10})) },
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, MonsterData::TVariant(int32_t(20)) },

			{ BestiaryEnum::MonsterAttribute::TSpeed, MonsterData::TVariant(int32_t(60)) },
			{ BestiaryEnum::MonsterAttribute::TRecoveryTime, MonsterData::TVariant(int32_t(6)) },
			{ BestiaryEnum::MonsterAttribute::TAbsorption, MonsterData::TVariant(int32_t(0)) },
			{ BestiaryEnum::MonsterAttribute::TSurpriseAdjust, MonsterData::TVariant(int32_t(2)) },

			{ BestiaryEnum::MonsterAttribute::TMonsterType, MonsterData::TVariant(BestiaryEnum::MonsterType::LivingCreature) },
			{ BestiaryEnum::MonsterAttribute::THabitat, MonsterData::TVariant(BestiaryEnum::Habitat::TEverywhere) },
			{ BestiaryEnum::MonsterAttribute::TLifestyle, MonsterData::TVariant(
				static_cast<BestiaryEnum::Lifestyle>(
					static_cast<int32_t>(BestiaryEnum::Lifestyle::TInstinctive) | 
					static_cast<int32_t>(BestiaryEnum::Lifestyle::TArachnid)
				)) },
			{ BestiaryEnum::MonsterAttribute::TWealthType, MonsterData::TVariant(BestiaryEnum::WealthType::TIncidental) },
			{ BestiaryEnum::MonsterAttribute::TAlignment, MonsterData::TVariant(BestiaryEnum::Alignment::TNeutral) },
			{ BestiaryEnum::MonsterAttribute::TCunning, MonsterData::TVariant(BestiaryEnum::Cunning::TLow) },
			{ BestiaryEnum::MonsterAttribute::TStrength, MonsterData::TVariant(BestiaryEnum::Strength::TBelowAverage) },
			{ BestiaryEnum::MonsterAttribute::TSize, MonsterData::TVariant(BestiaryEnum::Size::TMedium)},
			{ BestiaryEnum::MonsterAttribute::TSpecialCharacteristicData, MonsterData::TVariant(
				std::vector<SpecialCharacteristicData>({
					{ SpecialCharacteristics::TDarkVision },
					{ SpecialCharacteristics::TAttackCuresDisease },
				})
			)},

			{ BestiaryEnum::MonsterAttribute::TMonsterAttackData, MonsterData::TVariant(
				std::vector<MonsterAttackData>({
					{
						std::string(s_locale_key_attack_bite),
						RollData({0, 1, 4}),
						false,
						{{BestiaryEnum::PoisonType::TParalyzingVenom, 2}}
					}
				}))},

			{ BestiaryEnum::MonsterAttribute::TAttributePhysicalStrength, MonsterData::TVariant(int32_t(-1)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeStamina, MonsterData::TVariant(int32_t(-1)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeAgility, MonsterData::TVariant(int32_t(1)) },
			//TAttributeManualDexterity,
			{ BestiaryEnum::MonsterAttribute::TAttributePerception, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeWillpower, MonsterData::TVariant(int32_t(-4)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeFaith, MonsterData::TVariant(int32_t(-4)) },

			{ BestiaryEnum::MonsterAttribute::TIsEffectedByPoison, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedBySeverityDamage, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedByPysicalDamage, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedByMagicDamage, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedByFaithDamage, MonsterData::TVariant(int32_t(0)) },
		};

		result->_implementation->AddData(s_key_giant_spider_average, monster_data);
	}

	// s_key_giant_spider_below_average
	{
		std::shared_ptr<MonsterData> monster_data = std::make_shared<MonsterData>();
		monster_data->ParentID = s_key_giant_spider_average;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, MonsterData::TVariant(std::string(s_key_giant_spider_below_average))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, MonsterData::TVariant(int32_t(3)) },
			{ BestiaryEnum::MonsterAttribute::TDefence, MonsterData::TVariant(int32_t(22)) },
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, MonsterData::TVariant(RollData({10, 1, 10})) },
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, MonsterData::TVariant(int32_t(9)) },
		};

		result->_implementation->AddData(s_key_giant_spider_below_average, monster_data);
	}

	// s_key_giant_spider_above_average
	{
		std::shared_ptr<MonsterData> monster_data = std::make_shared<MonsterData>();
		monster_data->ParentID = s_key_giant_spider_average;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, MonsterData::TVariant(std::string(s_key_giant_spider_above_average))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, MonsterData::TVariant(int32_t(5)) },
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, MonsterData::TVariant(int32_t(7)) },
			{ BestiaryEnum::MonsterAttribute::TDefence, MonsterData::TVariant(int32_t(26)) },
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, MonsterData::TVariant(RollData({10, 5, 10})) },
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, MonsterData::TVariant(int32_t(54)) },

			{ BestiaryEnum::MonsterAttribute::TMonsterAttackData, MonsterData::TVariant(
				std::vector<MonsterAttackData>({
					{
						std::string(s_locale_key_attack_bite),
						RollData({0, 1, 6}),
						false,
						{{BestiaryEnum::PoisonType::TParalyzingVenom, 2}}
					}
				}))},
		};

		result->_implementation->AddData(s_key_giant_spider_above_average, monster_data);
	}

	// s_key_giant_spider_exceptional
	{
		std::shared_ptr<MonsterData> monster_data = std::make_shared<MonsterData>();
		monster_data->ParentID = s_key_giant_spider_exceptional;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, MonsterData::TVariant(std::string(s_key_giant_spider_exceptional))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, MonsterData::TVariant(int32_t(7)) },
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, MonsterData::TVariant(int32_t(9)) },
			{ BestiaryEnum::MonsterAttribute::TDefence, MonsterData::TVariant(int32_t(28)) },
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, MonsterData::TVariant(RollData({15, 7, 10})) },
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, MonsterData::TVariant(int32_t(162)) },

			{ BestiaryEnum::MonsterAttribute::TMonsterAttackData, MonsterData::TVariant(
				std::vector<MonsterAttackData>({
					{
						std::string(s_locale_key_attack_bite),
						RollData({0, 1, 6}),
						false,
						{{BestiaryEnum::PoisonType::TParalyzingVenom, 2}}
					}
				}))},
		};

		result->_implementation->AddData(s_key_giant_spider_exceptional, monster_data);
	}

	return result;
}

const std::string StaticLq::BestiaryPool::GetTestDataKeyGiantAnt()
{
	return s_key_giant_ant_warrior;
}

const std::string StaticLq::BestiaryPool::GetTestDataKeyGiantSpider()
{
	return s_key_giant_spider_average;
}

//static std::shared_ptr<BestiaryPool> FactoryFromFile(const std::filesystem::path& in_file_path);

StaticLq::BestiaryPool::BestiaryPool()
{
	_implementation = std::make_unique<BestiaryPoolImplementation>();
	return;
}

//void AddData(const std::string& in_key, const std::shared_ptr<MonsterData>& in_monster_data);
std::shared_ptr<StaticLq::ICombatant> StaticLq::BestiaryPool::MakeCombatant(
	const std::string& in_key,
	StaticLq::NameSystem& in_name_system, 
	LocaleSystem& in_locale_system 
	)
{
	return _implementation->MakeCombatant(
		in_key,
		in_name_system, 
		in_locale_system 
		);
}
