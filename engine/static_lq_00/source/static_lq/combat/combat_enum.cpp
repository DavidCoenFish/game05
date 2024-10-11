#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/combat_enum.h"

namespace
{
	struct Data
	{
		const char* const _data;
		const StaticLq::CombatEnum::CombatantValue _enum;
	};

	const char* const s_default_data = "";

	static const Data s_data[] = {
		{s_default_data, StaticLq::CombatEnum::CombatantValue::TDefault},

		{"DisplayName", StaticLq::CombatEnum::CombatantValue::TDisplayName},
		{"Self", StaticLq::CombatEnum::CombatantValue::TSelf},
		{"CanContinueCombat", StaticLq::CombatEnum::CombatantValue::TCanContinueCombat},
		{"MelleeInitiative", StaticLq::CombatEnum::CombatantValue::TMelleeInitiative},
		{"Defense", StaticLq::CombatEnum::CombatantValue::TDefense},
		{"AttackBonus", StaticLq::CombatEnum::CombatantValue::TAttackBonus},
		{"RecoveryTime", StaticLq::CombatEnum::CombatantValue::TRecoveryTime},
		{"SusceptibleSeverityDamage", StaticLq::CombatEnum::CombatantValue::TSusceptibleSeverityDamage},
		{"SusceptibleFaithDamage", StaticLq::CombatEnum::CombatantValue::TSusceptibleFaithDamage},
		{"Alignment", StaticLq::CombatEnum::CombatantValue::TAlignment},
		{"SusceptibleNonMagicalDamage", StaticLq::CombatEnum::CombatantValue::TSusceptibleNonMagicalDamage},
		{"Absorption", StaticLq::CombatEnum::CombatantValue::TAbsorption},
	};
	const int s_data_count = ARRAY_LITERAL_SIZE(s_data);

	const std::map<std::string, StaticLq::CombatEnum::CombatantValue>& GetStringEnumMap()
	{
		static std::map<std::string, StaticLq::CombatEnum::CombatantValue> s_map;
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

	const std::map<StaticLq::CombatEnum::CombatantValue, std::string>& GetEnumStringMap()
	{
		static std::map<StaticLq::CombatEnum::CombatantValue, std::string> s_map;
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
const StaticLq::CombatEnum::CombatantValue EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumFromString(const std::string& in_locale)
{
	const auto& map = GetStringEnumMap();
	auto found = map.find(in_locale);
	if (found != map.end())
	{
		return found->second;
	}
	return StaticLq::CombatEnum::CombatantValue::TDefault;
}

template <> 
const std::string EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(const StaticLq::CombatEnum::CombatantValue in_locale)
{
	const auto& map = GetEnumStringMap();
	auto found = map.find(in_locale);
	if (found != map.end())
	{
		return found->second;
	}
	return s_default_data;
}


