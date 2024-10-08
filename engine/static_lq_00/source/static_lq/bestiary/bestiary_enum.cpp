#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/bestiary_enum.h"

namespace
{
	struct Data
	{
		const char* const _data;
		const StaticLq::BestiaryEnum::CombatantValueInternal _enum;
	};

	const char* const s_default_data = "";

	static const Data s_data[] = {
		{s_default_data, StaticLq::BestiaryEnum::CombatantValueInternal::TDefault},
		//{"Name", StaticLq::BestiaryEnum::CombatantValueInternal::TName},
		{"Species", StaticLq::BestiaryEnum::CombatantValueInternal::TSpecies},
		{"Variation", StaticLq::BestiaryEnum::CombatantValueInternal::TVariation},
		{"DamageTollerance", StaticLq::BestiaryEnum::CombatantValueInternal::TDamageTollerance},
		{"DamageTolleranceConstant", StaticLq::BestiaryEnum::CombatantValueInternal::TDamageTolleranceConstant},
		{"DamageTolleranceDiceCount", StaticLq::BestiaryEnum::CombatantValueInternal::TDamageTolleranceDiceCount},
		{"DamageTolleranceDiceSide", StaticLq::BestiaryEnum::CombatantValueInternal::TDamageTolleranceDiceSide},
		{"DamageSum", StaticLq::BestiaryEnum::CombatantValueInternal::TDamageSum},
		{"DamageFatigue", StaticLq::BestiaryEnum::CombatantValueInternal::TDamageFatigue},
		{"DamagePhysical", StaticLq::BestiaryEnum::CombatantValueInternal::TDamagePhysical},
		{"DamageParalyzation", StaticLq::BestiaryEnum::CombatantValueInternal::TDamageParalyzation},
		{"HealthPoints", StaticLq::BestiaryEnum::CombatantValueInternal::THealthPoints},
		{"Alive", StaticLq::BestiaryEnum::CombatantValueInternal::TAlive},
		{"AttackBonus", StaticLq::BestiaryEnum::CombatantValueInternal::TAttackBonus},
	};
	const int s_data_count = ARRAY_LITERAL_SIZE(s_data);

	const std::map<std::string, StaticLq::BestiaryEnum::CombatantValueInternal>& GetStringEnumMap()
	{
		static std::map<std::string, StaticLq::BestiaryEnum::CombatantValueInternal> s_map;
		if (0 == s_map.size())
		{
			for (int index = 0; index < s_data_count; ++index)
			{
				const Data& data = s_data[index];
				s_map[data._data] = data._enum;
			}
		}
		return s_map;
	}

	const std::map<StaticLq::BestiaryEnum::CombatantValueInternal, std::string>& GetEnumStringMap()
	{
		static std::map<StaticLq::BestiaryEnum::CombatantValueInternal, std::string> s_map;
		if (0 == s_map.size())
		{
			for (int index = 0; index < s_data_count; ++index)
			{
				const Data& data = s_data[index];
				s_map[data._enum] = data._data;
			}
		}
		return s_map;
	}
}

template <> 
const StaticLq::BestiaryEnum::CombatantValueInternal EnumSoftBind<StaticLq::BestiaryEnum::CombatantValueInternal>::EnumFromString(const std::string& in_locale)
{
	const auto& map = GetStringEnumMap();
	auto found = map.find(in_locale);
	if (found != map.end())
	{
		return found->second;
	}
	return StaticLq::BestiaryEnum::CombatantValueInternal::TDefault;
}

template <> 
const std::string EnumSoftBind<StaticLq::BestiaryEnum::CombatantValueInternal>::EnumToString(const StaticLq::BestiaryEnum::CombatantValueInternal in_locale)
{
	const auto& map = GetEnumStringMap();
	auto found = map.find(in_locale);
	if (found != map.end())
	{
		return found->second;
	}
	return s_default_data;
}



