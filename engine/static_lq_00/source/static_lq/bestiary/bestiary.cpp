#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/bestiary.h"

#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_enum.h"
#include "common/log/log.h"
#include "static_lq/bestiary/monster_data.h"
#include "static_lq/bestiary/special_characteristic_data.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"
#include "static_lq/name/name_system.h"
#include "static_lq/random_sequence.h"

/*
namespace
{
	constexpr char s_locale_key_species_name[] = "slqsc_bestiary_species_name";
	constexpr char s_locale_key_species_name_variation[] = "slqsc_bestiary_species_name_variation";
	constexpr char s_locale_key_species_giant_ant[] = "slqsc_bestiary_species_giant_ant";
	constexpr char s_locale_key_species_giant_spider[] = "slqsc_bestiary_species_giant_spider";
	constexpr char s_locale_key_attack_mandibles[] = "slqsc_bestiary_attack_mandibles";
	constexpr char s_locale_key_attack_bite[] = "slqsc_bestiary_attack_bite";
	constexpr char s_locale_key_example_below_average[] = "slqsc_bestiary_example_below_average";
	constexpr char s_locale_key_example_average[] = "slqsc_bestiary_example_average";
	constexpr char s_locale_key_example_above_average[] = "slqsc_bestiary_example_above_average";
	constexpr char s_locale_key_example_exceptional[] = "slqsc_bestiary_example_exceptional";
	constexpr char s_locale_key_example_worker_ant[] = "slqsc_bestiary_example_worker_ant";
	constexpr char s_locale_key_example_warrior_ant[] = "slqsc_bestiary_example_warrior_ant";
	constexpr char s_locale_key_example_dew_pot_worker_ant[] = "slqsc_bestiary_example_dew_pot_worker_ant";
	constexpr char s_locale_key_example_queen_ant[] = "slqsc_bestiary_example_queen_ant";

	constexpr char s_locale_key_self_tooltip[] = "slqsc_self";
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

	void DagAddName(DagThreadedCollection& in_dag_collection, const std::string& in_name_key, const std::string& in_species, const std::string& in_variation)
	{
		auto name_key = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDisplayName),
			DagThreadedHelper::CreateDagValue<std::string>(in_name_key)
			);
		auto species = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSpecies),
			DagThreadedHelper::CreateDagValue<std::string>(in_species)
			);
		auto variation = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TVariation),
			DagThreadedHelper::CreateDagValue<std::string>(in_variation)
			);

		// or a calculate node so that the tooltip changes on input change
		auto self = in_dag_collection.CreateNodeCalculate(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSelf),
			[](
				const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >&
			) -> std::shared_ptr< IDagThreadedValue >
			{
				return DagThreadedHelper::CreateDagValue<std::string>(0);
			},
			"",
			s_locale_key_self_tooltip
			);
		in_dag_collection.AddNodeLinkStack(self, name_key);
		in_dag_collection.AddNodeLinkStack(self, species);
		in_dag_collection.AddNodeLinkStack(self, variation);
	}

	void DagAddDamageTolerance(DagThreadedCollection& in_dag_collection, const int32_t in_id, const StaticLq::MonsterVariationData& in_monster_variation_data)
	{
		auto damage_tolerance_seed = in_dag_collection.CreateNodeVariable(
			in_dag_collection.MakeUid(), 
			DagThreadedHelper::CreateDagValue<int32_t>(in_id)
			);
		auto damage_tolerance_constant = in_dag_collection.CreateNodeVariable(
		EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTolleranceConstant),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._damage_tolerance._constant)
			);
		auto damage_tolerance_dice_count = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTolleranceDiceCount),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._damage_tolerance._dice_count)
			);
		auto damage_tolerance_dice_base = in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDamageTolleranceDiceSide),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._damage_tolerance._dice_base)
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

	void DagAddCombatVariables(DagThreadedCollection& in_dag_collection, const StaticLq::MonsterData& in_monster_data, const StaticLq::MonsterVariationData& in_monster_variation_data)
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
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._combat_level)
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TExperencePointValue),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._experence_point_value)
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TAttackBonus),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._attack_bonus)
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDefense),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._defence)
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TRecoveryTime),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_data._recovery_time)
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptibleSeverityDamage),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_data._is_living_creature)
			);
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptiblePoison),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_data._is_living_creature)
			);
		//TSusceptibleFaithDamage, // combat
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptibleFaithDamage),
			DagThreadedHelper::CreateDagValue<int32_t>(false)
			);
		//TSusceptibleNonMagicalDamage, // combat
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptibleNonMagicalDamage),
			DagThreadedHelper::CreateDagValue<int32_t>(true)
			);
		//TSusceptibleMagicalDamage, // combat
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSusceptibleMagicalDamage),
			DagThreadedHelper::CreateDagValue<int32_t>(true)
			);

		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TAbsorption),
			DagThreadedHelper::CreateDagValue<int32_t>(in_monster_data._absorption)
			);

		//TAlignment
		in_dag_collection.CreateNodeVariable(
			EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TAlignment),
			DagThreadedHelper::CreateDagValue<int32_t>(static_cast<int32_t>(in_monster_data._alignment))
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

	void DagAddAttributes(DagThreadedCollection& in_dag_collection, const StaticLq::MonsterData& in_monster_data)
	{
		AddAttribute(in_dag_collection, in_monster_data._attribute_data._agility, StaticLq::CombatEnum::CombatantValue::TRawAgility, StaticLq::CombatEnum::CombatantValue::TBuffAgility, StaticLq::CombatEnum::CombatantValue::TAgility, s_locale_key_agility);
		AddAttribute(in_dag_collection, in_monster_data._attribute_data._faith, StaticLq::CombatEnum::CombatantValue::TRawFaith, StaticLq::CombatEnum::CombatantValue::TBuffFaith, StaticLq::CombatEnum::CombatantValue::TFaith, s_locale_key_faith);
		AddAttribute(in_dag_collection, in_monster_data._attribute_data._manual_dexterity, StaticLq::CombatEnum::CombatantValue::TRawManualDexterity, StaticLq::CombatEnum::CombatantValue::TBuffManualDexterity, StaticLq::CombatEnum::CombatantValue::TManualDexterity, s_locale_key_manual_dexterity);
		AddAttribute(in_dag_collection, in_monster_data._attribute_data._perception, StaticLq::CombatEnum::CombatantValue::TRawPerception, StaticLq::CombatEnum::CombatantValue::TBuffPerception, StaticLq::CombatEnum::CombatantValue::TPerception, s_locale_key_perception);
		AddAttribute(in_dag_collection, in_monster_data._attribute_data._physical_strength, StaticLq::CombatEnum::CombatantValue::TRawPhysicalStrength, StaticLq::CombatEnum::CombatantValue::TBuffPhysicalStrength, StaticLq::CombatEnum::CombatantValue::TPhysicalStrength, s_locale_key_physical_strength);
		AddAttribute(in_dag_collection, in_monster_data._attribute_data._stamina, StaticLq::CombatEnum::CombatantValue::TRawStamina, StaticLq::CombatEnum::CombatantValue::TBuffStamina, StaticLq::CombatEnum::CombatantValue::TStamina, s_locale_key_stamina);
		AddAttribute(in_dag_collection, in_monster_data._attribute_data._willpower, StaticLq::CombatEnum::CombatantValue::TRawWillpower, StaticLq::CombatEnum::CombatantValue::TBuffWillpower, StaticLq::CombatEnum::CombatantValue::TWillpower, s_locale_key_willpower);
	}

	std::shared_ptr<DagThreadedCollection> MakeMonsterDag(const int32_t in_id, const StaticLq::MonsterData& in_monster_data, const int32_t in_variation_index, const std::string& in_name_key)
	{
		//StaticLq::RandomSequence random_sequence(in_id);
		std::shared_ptr<DagThreadedCollection> result = DagThreadedCollection::Factory();

		DagAddName(*result, in_name_key, in_monster_data._species, in_monster_data._array_variation[in_variation_index]._display_name);

		DagAddDamageTolerance(*result, in_id, in_monster_data._array_variation[in_variation_index]);
		DagAddDamageSum(*result);
		DagAddStatus(*result);
		DagAddCanContinueCombat(*result);
		DagAddCombatVariables(*result, in_monster_data, in_monster_data._array_variation[in_variation_index]);
		DagAddAttributes(*result, in_monster_data);

		#if 0
		auto damage_tollerace = result->FindNode(s_dag_key_damage_tolerance_raw);
		const int32_t hp = DagThreadedHelper::GetValue<int32_t>(result->GetDagValue(damage_tollerace));
		LOG_CONSOLE("MakeMonsterDag hp:[%d]", hp);
		#endif

		return result;
	}
}

/// somewhere for locale data for the factory default combatabts to be specified
void StaticLq::Bestiary::RegisterLocaleSystem(LocaleSystem& in_out_locale_system)
{
	const std::vector<LocaleSystem::Data> data = {
		{s_locale_key_species_name, "{species}"},
		{s_locale_key_species_name_variation, "{species} ({variation})"},
		{s_locale_key_species_giant_ant, "Giant Ant"},
		{s_locale_key_species_giant_spider, "Giant Spider"},
		{s_locale_key_example_below_average, "Below Average"},
		{s_locale_key_example_average, "Average"},
		{s_locale_key_example_above_average, "Above Average"},
		{s_locale_key_example_exceptional, "Exceptional"},
		{s_locale_key_example_worker_ant, "Worker Ant"},
		{s_locale_key_example_warrior_ant, "Warrior Ant"},
		{s_locale_key_example_dew_pot_worker_ant, "Dew-Pot Worker Ant"},
		{s_locale_key_example_queen_ant, "Queen Ant"},

		{s_locale_key_attack_mandibles, "mandibles"},
		{s_locale_key_attack_bite, "bite"},

		{s_locale_key_self_tooltip, "{Name} the {Species}, {TVariation}"},
		{s_locale_key_damage_tolerance, "Damage Tolerance"},
		{s_locale_key_damage_tolerance_tooltip, "{self} = {index.1} + {index.2}d{index.3}"},
		{s_locale_key_damage_sum, "Damage sum"},
		{s_locale_key_damage_paralyzation_sum, "Damage paralyzation sum"},
		{s_locale_key_sum_tooltip, "{self} = {index.1} + {index.2}"},
		{s_locale_key_fatigue_damage, "Fatigue damage"},
		{s_locale_key_physical_damage, "Physical damage"},
		{s_locale_key_paralyzation_damage, "Paralyzation damage"},
		{s_locale_key_alive, "Alive"},
		{s_locale_key_paralyzed, "Paralyzed"},
		{s_locale_key_0_less_i0_minus_i1, "{self} = 0 <= {index.0} - {index.0}"},

		{s_locale_key_can_continue_combat, "Can continue combat"},
		{s_locale_key_can_continue_combat_tooltip, "{self} = {index.0} and not {index.1}"},

		{s_locale_key_stack_sum_tooltip, "{self} = {stack_sum}"},

		{s_locale_key_physical_strength, "Physical strength"},
		{s_locale_key_stamina, "Stamina"},
		{s_locale_key_agility, "Agility"},
		{s_locale_key_manual_dexterity, "Manual dexterity"},
		{s_locale_key_perception, "Perception"},
		{s_locale_key_willpower, "Willpower"},
		{s_locale_key_faith, "Faith"},

		};

	in_out_locale_system.Append(LocaleISO_639_1::Default, data);
	//in_out_locale_system.Append(LocaleISO_639_1::English, data);
}

//tome_of_terrors.pdf page:142
std::shared_ptr<StaticLq::ICombatant> StaticLq::Bestiary::FactoryDefaultGiantSpider(
	NameSystem& in_name_system, 
	LocaleSystem& in_locale_system 
	)
{
	const MonsterData monster_data = {
		s_locale_key_species_giant_spider,
		static_cast<BestiaryEnum::Habitat>(
			static_cast<int32_t>(BestiaryEnum::Habitat::TForest) | 
			static_cast<int32_t>(BestiaryEnum::Habitat::TSubterranean) | 
			static_cast<int32_t>(BestiaryEnum::Habitat::TDesert)
			),
		static_cast<BestiaryEnum::Lifestyle>(
			static_cast<int32_t>(BestiaryEnum::Lifestyle::TInstinctive) | 
			static_cast<int32_t>(BestiaryEnum::Lifestyle::TArachnid)
			),
		BestiaryEnum::WealthType::TIncidental,
		BestiaryEnum::Alignment::TNeutral,
		BestiaryEnum::Cunning::TLow,
		60,
		BestiaryEnum::Strength::TBelowAverage,
		BestiaryEnum::Size::TMedium,
		{
			{SpecialCharacteristics::TAttackCuresDisease},
			{SpecialCharacteristics::TDarkVision}
		},
		6,
		0,
		2,
		true,
		{
			std::optional<int32_t>(-1),
			std::optional<int32_t>(-1),
			std::optional<int32_t>(1),
			std::nullopt,
			std::optional<int32_t>(1),
			std::optional<int32_t>(-4),
			std::optional<int32_t>(-4)
		},
		std::vector<MonsterVariationData>({
			MonsterVariationData({
				s_locale_key_example_below_average,
				1,
				3,
				22,
				{10, 1, 10},
				9,
				std::vector<MonsterAttackData>({
					{
						s_locale_key_attack_bite,
						{0,1,4},
						false,
						std::vector<MonsterAttackData::EffectData>({
							{ StaticLq::BestiaryEnum::PoisonType::TParalyzingVenom, 2 }
							})
					}
				})
			}),
			MonsterVariationData({
				s_locale_key_example_average,
				3,
				5,
				24,
				{10, 3, 10},
				20,
				std::vector<MonsterAttackData>({
					{
						s_locale_key_attack_bite,
						{0,1,4},
						false,
						std::vector<MonsterAttackData::EffectData>({
							{ StaticLq::BestiaryEnum::PoisonType::TParalyzingVenom, 2 }
							})
					}
				})
			}),
			MonsterVariationData({
				s_locale_key_example_above_average,
				5,
				7,
				26,
				{10, 5, 10},
				54,
				std::vector<MonsterAttackData>({
					{
						s_locale_key_attack_bite,
						{0,1,6},
						false,
						std::vector<MonsterAttackData::EffectData>({
							{ StaticLq::BestiaryEnum::PoisonType::TParalyzingVenom, 2 }
							})
					}
				})
			}),
			MonsterVariationData({
				s_locale_key_example_above_average,
				7,
				9,
				28,
				{15, 7, 10},
				162,
				std::vector<MonsterAttackData>({
					{
						s_locale_key_attack_bite,
						{0,1,6},
						false,
						std::vector<MonsterAttackData::EffectData>({
							{ StaticLq::BestiaryEnum::PoisonType::TParalyzingVenom, 2 }
							})
					}
				})
			})
		})
	};

	const int id = ICombatant::MakeNewId();
	const std::string name_key = in_name_system.GenerateName(StaticLq::NameSystem::GetKeyGiantSpider(), id, in_locale_system);
	const int32_t variation_index = 1;
	std::shared_ptr<DagThreadedCollection> dag_collection = MakeMonsterDag(id, monster_data, variation_index, name_key);

	return std::make_shared<SimpleCombatMonster>(id, dag_collection, monster_data._array_variation[variation_index]._array_attack_data);
}

//tome_of_terrors.pdf page:32
std::shared_ptr<StaticLq::ICombatant> StaticLq::Bestiary::FactoryDefaultGiantAnt(
	NameSystem& in_name_system, 
	LocaleSystem& in_locale_system 
	)
{
	const MonsterData monster_data = {
		s_locale_key_species_giant_ant,
		BestiaryEnum::Habitat::TEverywhere,
		static_cast<BestiaryEnum::Lifestyle>(
			static_cast<int32_t>(BestiaryEnum::Lifestyle::TInstinctive) | 
			static_cast<int32_t>(BestiaryEnum::Lifestyle::TComunal) | 
			static_cast<int32_t>(BestiaryEnum::Lifestyle::TInsect)
			),
		BestiaryEnum::WealthType::TMineral,
		BestiaryEnum::Alignment::TNeutral,
		BestiaryEnum::Cunning::TLow,
		70,
		BestiaryEnum::Strength::THigh,
		BestiaryEnum::Size::TSmall,
		{},
		7,
		6,
		0,
		true,
		{
			std::optional<int32_t>(6),
			std::optional<int32_t>(6),
			std::optional<int32_t>(0),
			std::nullopt,
			std::optional<int32_t>(0),
			std::optional<int32_t>(-3),
			std::optional<int32_t>(-3)
		},
		std::vector<MonsterVariationData>({
			MonsterVariationData({
				s_locale_key_example_worker_ant,
				1,
				3,
				21,
				{15, 1, 10},
				15,
				std::vector<MonsterAttackData>({
					{
						s_locale_key_attack_mandibles,
						{0,2,4},
						true,
						{}
					}
				})
			}),
			MonsterVariationData({
				s_locale_key_example_warrior_ant,
				4,
				6,
				22,
				{15, 4, 10},
				35,
				std::vector<MonsterAttackData>({
					{
						s_locale_key_attack_mandibles,
						{0,5,4},
						true,
						{}
					}
				})
			}),
			MonsterVariationData({
				s_locale_key_example_dew_pot_worker_ant,
				0,
				0,
				5,
				{10, 1, 10},
				1,
				std::vector<MonsterAttackData>({
					{
						s_locale_key_attack_mandibles,
						{0,2,4},
						true,
						{}
					}
				})
				// set speed 10
			}),
			MonsterVariationData({
				s_locale_key_example_queen_ant,
				0,
				0,
				5,
				{80, 2, 10},
				2,
				std::vector<MonsterAttackData>({
					{
						s_locale_key_attack_mandibles,
						{0,2,4},
						true,
						{}
					}
				})
				// set speed 0
			})
		})
	};

	const int id = ICombatant::MakeNewId();
	const std::string name_key = in_name_system.GenerateName(StaticLq::NameSystem::GetKeyGiantAnt(), id, in_locale_system);

	const int32_t variation_index = 1;
	std::shared_ptr<DagThreadedCollection> dag_collection = MakeMonsterDag(id, monster_data, variation_index, name_key);

	return std::make_shared<SimpleCombatMonster>(id, dag_collection, monster_data._array_variation[variation_index]._array_attack_data);
}

*/