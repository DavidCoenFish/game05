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
	constexpr char s_locale_key_self_tooltip[] = "slqsc_self_tooltip";
	constexpr char s_locale_key_species_giant_ant[] = "slqsc_species_giant_ant";
	constexpr char s_locale_key_description_giant_ant[] = "slqsc_species_giant_ant";
	constexpr char s_locale_key_attack_mandibles[] = "slqsc_bestiary_attack_mandibles";
	constexpr char s_locale_key_attack_bite[] = "slqsc_bestiary_attack_bite";
	constexpr char s_locale_key_damage_tolerance[] = "slqsc_damage_tolerance";
	constexpr char s_locale_key_damage_tolerance_tooltip[] = "slqsc_damage_tolerance_tooltip";

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

	std::shared_ptr<DagThreadedCollection> MakeMonsterDag(const int32_t in_id, const std::shared_ptr<StaticLq::MonsterData>& in_monster_data, const std::string& in_name_key)
	{
		std::shared_ptr<DagThreadedCollection> result = DagThreadedCollection::Factory();

		DagAddName(*result, in_name_key, GetRef<std::string>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TSpeciesName), GetRef<std::string>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TVariationName));
		DagAddDamageTolerance(*result, in_id, GetRef<StaticLq::RollData>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TDamageToleranceRollData));
		//DagAddDamageSum(*result);
		//DagAddStatus(*result);
		//DagAddCanContinueCombat(*result);
		//DagAddCombatVariables(*result, in_monster_data, in_monster_data._array_variation[in_variation_index]);
		//DagAddAttributes(*result, in_monster_data);

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
		{s_locale_key_self_tooltip, "{Name} the {Species}, {TVariation}"},
		{s_locale_key_species_giant_ant, "Giant Ant"},
		{s_locale_key_description_giant_ant, "giant ant description text"},
		{s_locale_key_attack_mandibles, "mandibles"},
		{s_locale_key_attack_bite, "bite"},
		{s_locale_key_damage_tolerance, "Damage Tolerance"},
		{s_locale_key_damage_tolerance_tooltip, "{self} = {index.1} + {index.2}d{index.3}"},
		};

	in_out_locale_system.Append(LocaleISO_639_1::Default, data);
}

std::shared_ptr<StaticLq::BestiaryPool> StaticLq::BestiaryPool::FactoryWithTestData()
{
	std::shared_ptr<StaticLq::BestiaryPool> result = std::make_shared<StaticLq::BestiaryPool>();

	// s_key_giant_ant
	{
		std::shared_ptr<StaticLq::MonsterData> monster_data = std::make_shared<StaticLq::MonsterData>();
		monster_data->MapAttibute = {
			//TVariationName, // "average", "warrior", "queen", "worker"
			{ BestiaryEnum::MonsterAttribute::TNameKey, StaticLq::MonsterData::TVariant(StaticLq::NameSystem::GetKeyGiantAnt())},
			{ BestiaryEnum::MonsterAttribute::TSpeciesName, StaticLq::MonsterData::TVariant(std::string(s_locale_key_species_giant_ant)) },
			{ BestiaryEnum::MonsterAttribute::TDescription, StaticLq::MonsterData::TVariant(std::string(s_locale_key_description_giant_ant)) },

			//TCombatLevel,
			//TAttackBonus,
			//TDefence,
			//TDamageToleranceRollData,
			//TExperencePointValue,

			{ BestiaryEnum::MonsterAttribute::TSpeed, StaticLq::MonsterData::TVariant(int32_t(70)) },
			{ BestiaryEnum::MonsterAttribute::TRecoveryTime, StaticLq::MonsterData::TVariant(int32_t(7)) },
			{ BestiaryEnum::MonsterAttribute::TAbsorption, StaticLq::MonsterData::TVariant(int32_t(6)) },
			{ BestiaryEnum::MonsterAttribute::TSurpriseAdjust, StaticLq::MonsterData::TVariant(int32_t(0)) },

			{ BestiaryEnum::MonsterAttribute::TMonsterType, StaticLq::MonsterData::TVariant(StaticLq::BestiaryEnum::MonsterType::LivingCreature) },
			{ BestiaryEnum::MonsterAttribute::THabitat, StaticLq::MonsterData::TVariant(StaticLq::BestiaryEnum::Habitat::TEverywhere) },
			{ BestiaryEnum::MonsterAttribute::TLifestyle, StaticLq::MonsterData::TVariant(
				static_cast<StaticLq::BestiaryEnum::Lifestyle>(
					static_cast<int32_t>(StaticLq::BestiaryEnum::Lifestyle::TInstinctive) | 
					static_cast<int32_t>(StaticLq::BestiaryEnum::Lifestyle::TComunal) | 
					static_cast<int32_t>(StaticLq::BestiaryEnum::Lifestyle::TInsect) 
				)) },
			{ BestiaryEnum::MonsterAttribute::TWealthType, StaticLq::MonsterData::TVariant(StaticLq::BestiaryEnum::WealthType::TMineral) },
			{ BestiaryEnum::MonsterAttribute::TAlignment, StaticLq::MonsterData::TVariant(StaticLq::BestiaryEnum::Alignment::TNeutral) },
			{ BestiaryEnum::MonsterAttribute::TCunning, StaticLq::MonsterData::TVariant(StaticLq::BestiaryEnum::Cunning::TLow) },
			{ BestiaryEnum::MonsterAttribute::TStrength, StaticLq::MonsterData::TVariant(StaticLq::BestiaryEnum::Strength::THigh) },
			{ BestiaryEnum::MonsterAttribute::TSize, StaticLq::MonsterData::TVariant(
				static_cast<StaticLq::BestiaryEnum::Size>(
					static_cast<int32_t>(StaticLq::BestiaryEnum::Size::TSmall) | 
					static_cast<int32_t>(StaticLq::BestiaryEnum::Size::TMedium) 
				))},
			//TSpecialCharacteristicData,

			{ BestiaryEnum::MonsterAttribute::TMonsterAttackData, StaticLq::MonsterData::TVariant(
				std::vector<MonsterAttackData>({
					{
						std::string(s_locale_key_attack_mandibles),
						RollData({0, 2, 4}),
						true
					}
				}))},

			{ BestiaryEnum::MonsterAttribute::TAttributePhysicalStrength, StaticLq::MonsterData::TVariant(int32_t(6)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeStamina, StaticLq::MonsterData::TVariant(int32_t(6)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeAgility, StaticLq::MonsterData::TVariant(int32_t(0)) },
			//TAttributeManualDexterity,
			{ BestiaryEnum::MonsterAttribute::TAttributePerception, StaticLq::MonsterData::TVariant(int32_t(0)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeWillpower, StaticLq::MonsterData::TVariant(int32_t(-3)) },
			{ BestiaryEnum::MonsterAttribute::TAttributeFaith, StaticLq::MonsterData::TVariant(int32_t(-3)) },

			{ BestiaryEnum::MonsterAttribute::TIsEffectedByPoison, StaticLq::MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedBySeverityDamage, StaticLq::MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedByPysicalDamage, StaticLq::MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedByMagicDamage, StaticLq::MonsterData::TVariant(int32_t(1)) },
			{ BestiaryEnum::MonsterAttribute::TIsEffectedByFaithDamage, StaticLq::MonsterData::TVariant(int32_t(0)) },
		};

		result->_implementation->AddData(s_key_giant_ant, monster_data);
	}

	// s_key_giant_ant_worker
	{
		std::shared_ptr<StaticLq::MonsterData> monster_data = std::make_shared<StaticLq::MonsterData>();
		monster_data->ParentID = s_key_giant_ant;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, StaticLq::MonsterData::TVariant(std::string(s_key_giant_ant_worker))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, StaticLq::MonsterData::TVariant(int32_t(1))},
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, StaticLq::MonsterData::TVariant(int32_t(3))},
			{ BestiaryEnum::MonsterAttribute::TDefence, StaticLq::MonsterData::TVariant(int32_t(21))},
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, StaticLq::MonsterData::TVariant(StaticLq::RollData({15, 1, 10}))},
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, StaticLq::MonsterData::TVariant(int32_t(15))},
		};

		result->_implementation->AddData(s_key_giant_ant_worker, monster_data);
	}

	// s_key_giant_ant_warrior
	{
		std::shared_ptr<StaticLq::MonsterData> monster_data = std::make_shared<StaticLq::MonsterData>();
		monster_data->ParentID = s_key_giant_ant;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, StaticLq::MonsterData::TVariant(std::string(s_key_giant_ant_warrior))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, StaticLq::MonsterData::TVariant(int32_t(4))},
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, StaticLq::MonsterData::TVariant(int32_t(6))},
			{ BestiaryEnum::MonsterAttribute::TDefence, StaticLq::MonsterData::TVariant(int32_t(22))},
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, StaticLq::MonsterData::TVariant(StaticLq::RollData({15, 4, 10}))},
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, StaticLq::MonsterData::TVariant(int32_t(35))},

			{ BestiaryEnum::MonsterAttribute::TMonsterAttackData, StaticLq::MonsterData::TVariant(
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
		std::shared_ptr<StaticLq::MonsterData> monster_data = std::make_shared<StaticLq::MonsterData>();
		monster_data->ParentID = s_key_giant_ant;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, StaticLq::MonsterData::TVariant(std::string(s_key_giant_ant_dew_pot_worker))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, StaticLq::MonsterData::TVariant(int32_t(0))},
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, StaticLq::MonsterData::TVariant(int32_t(0))},
			{ BestiaryEnum::MonsterAttribute::TDefence, StaticLq::MonsterData::TVariant(int32_t(5))},
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, StaticLq::MonsterData::TVariant(StaticLq::RollData({15, 1, 10}))},
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, StaticLq::MonsterData::TVariant(int32_t(1))},
		};

		result->_implementation->AddData(s_key_giant_ant_dew_pot_worker, monster_data);
	}

	// s_key_giant_ant_queen
	{
		std::shared_ptr<StaticLq::MonsterData> monster_data = std::make_shared<StaticLq::MonsterData>();
		monster_data->ParentID = s_key_giant_ant;
		monster_data->MapAttibute = {
			{ BestiaryEnum::MonsterAttribute::TVariationName, StaticLq::MonsterData::TVariant(std::string(s_key_giant_ant_queen))},

			{ BestiaryEnum::MonsterAttribute::TCombatLevel, StaticLq::MonsterData::TVariant(int32_t(0))},
			{ BestiaryEnum::MonsterAttribute::TAttackBonus, StaticLq::MonsterData::TVariant(int32_t(0))},
			{ BestiaryEnum::MonsterAttribute::TDefence, StaticLq::MonsterData::TVariant(int32_t(5))},
			{ BestiaryEnum::MonsterAttribute::TDamageToleranceRollData, StaticLq::MonsterData::TVariant(StaticLq::RollData({80, 2, 10}))},
			{ BestiaryEnum::MonsterAttribute::TExperencePointValue, StaticLq::MonsterData::TVariant(int32_t(2))},
		};

		result->_implementation->AddData(s_key_giant_ant_queen, monster_data);
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
