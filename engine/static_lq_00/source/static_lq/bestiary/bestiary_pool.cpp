#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/bestiary_pool.h"
#include "static_lq/bestiary/monster_data.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"
#include "static_lq/name/name_system.h"
#include "common/locale/locale_system.h"
#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"

namespace
{
	//constexpr char s_locale_key_self_tooltip[] = "slqsc_self";


template<typename TYPE> const TYPE& GetRef(const StaticLq::MonsterData& in_monster_data, const StaticLq::BestiaryEnum::MonsterAttribute in_attribute)
{
	auto found = monster_data.MapAttibute.find(in_attribute);
	if (found != monster_data.MapAttibute.end())
	{
		return std::get<TYPE>(found->second);
	}
	static const TYPE result = {};
	return result;
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
		""//s_locale_key_self_tooltip
		);
	in_dag_collection.AddNodeLinkStack(self, name_key);
	in_dag_collection.AddNodeLinkStack(self, species);
	in_dag_collection.AddNodeLinkStack(self, variation);
}

std::shared_ptr<DagThreadedCollection> MakeMonsterDag(const int32_t in_id, const StaticLq::MonsterData& in_monster_data, const std::string& in_name_key)
{
	std::shared_ptr<DagThreadedCollection> result = DagThreadedCollection::Factory();

	//DagAddName(*result, in_name_key, in_monster_data._species, in_monster_data._array_variation[in_variation_index]._display_name);
	DagAddName(*result, in_name_key, GetRef<std::string>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TSpeciesName), GetRef<std::string>(in_monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TVariationName));
	//DagAddDamageTolerance(*result, in_id, in_monster_data._array_variation[in_variation_index]);
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
			GetRef<std::string>(*monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TNameKey),
			id, 
			in_locale_system
			);
		const int32_t variation_index = 1;
		auto dag_collection = MakeMonsterDag(id, *monster_data, name_key);
		auto result = std::make_shared<StaticLq::SimpleCombatMonster>(
			id, 
			dag_collection, 
			GetRef<std::vector<StaticLq::MonsterAttackData>>(*monster_data, StaticLq::BestiaryEnum::MonsterAttribute::TMonsterAttackData)
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

std::shared_ptr<StaticLq::BestiaryPool> StaticLq::BestiaryPool::FactoryWithTestData()
{
}
const std::string StaticLq::BestiaryPool::GetTestDataKeyGiantAnt(){}
const std::string StaticLq::BestiaryPool::GetTestDataKeyGiantSpider()
{
	
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
