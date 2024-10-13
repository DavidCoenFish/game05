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

		{"Self", StaticLq::CombatEnum::CombatantValue::TSelf},
		{"DisplayName", StaticLq::CombatEnum::CombatantValue::TDisplayName},
		{"Species", StaticLq::CombatEnum::CombatantValue::TSpecies},
		{"Variation", StaticLq::CombatEnum::CombatantValue::TVariation},

		{"DamageTollerance", StaticLq::CombatEnum::CombatantValue::TDamageTollerance},
		{"DamageTolleranceConstant", StaticLq::CombatEnum::CombatantValue::TDamageTolleranceConstant},
		{"DamageTolleranceDiceCount", StaticLq::CombatEnum::CombatantValue::TDamageTolleranceDiceCount},
		{"DamageTolleranceDiceSide", StaticLq::CombatEnum::CombatantValue::TDamageTolleranceDiceSide},
		{"DamageSum", StaticLq::CombatEnum::CombatantValue::TDamageSum},
		{"DamageFatigue", StaticLq::CombatEnum::CombatantValue::TDamageFatigue},
		{"DamagePhysical", StaticLq::CombatEnum::CombatantValue::TDamagePhysical},		
		{"DamageParalyzationSum", StaticLq::CombatEnum::CombatantValue::TDamageParalyzationSum},
		{"DamageParalyzation", StaticLq::CombatEnum::CombatantValue::TDamageParalyzation},
		{"HealthPoints", StaticLq::CombatEnum::CombatantValue::THealthPoints},
		{"IsAlive", StaticLq::CombatEnum::CombatantValue::TIsAlive},
		{"IsParalyzed", StaticLq::CombatEnum::CombatantValue::TIsParalyzed},

		{"CanContinueCombat", StaticLq::CombatEnum::CombatantValue::TCanContinueCombat},
		{"MelleeInitiative", StaticLq::CombatEnum::CombatantValue::TMelleeInitiative},
		{"Defense", StaticLq::CombatEnum::CombatantValue::TDefense},
		{"AttackBonus", StaticLq::CombatEnum::CombatantValue::TAttackBonus},
		{"RecoveryTime", StaticLq::CombatEnum::CombatantValue::TRecoveryTime},
		{"SusceptibleSeverityDamage", StaticLq::CombatEnum::CombatantValue::TSusceptibleSeverityDamage},
		{"SusceptiblePoison", StaticLq::CombatEnum::CombatantValue::TSusceptiblePoison},
		{"SusceptibleFaithDamage", StaticLq::CombatEnum::CombatantValue::TSusceptibleFaithDamage},
		{"SusceptibleNonMagicalDamage", StaticLq::CombatEnum::CombatantValue::TSusceptibleNonMagicalDamage},
		{"SusceptibleMagicalDamage", StaticLq::CombatEnum::CombatantValue::TSusceptibleMagicalDamage},
		{"Absorption", StaticLq::CombatEnum::CombatantValue::TAbsorption},
		{"Alignment", StaticLq::CombatEnum::CombatantValue::TAlignment},

		{"PoisonDoseCountToxin", StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountToxin},
		{"PoisonDoseCountHallucinogen", StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountHallucinogen},
		{"PoisonDoseCountVenom", StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountVenom},
		{"PoisonDoseCountKillingVenom", StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountKillingVenom},
		{"PoisonDoseCountParalyzingVenom", StaticLq::CombatEnum::CombatantValue::TPoisonDoseCountParalyzingVenom},
		{"PoisonSaveFailedToxin", StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedToxin},
		{"PoisonSaveFailedHallucinogen", StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedHallucinogen},
		{"PoisonSaveFailedVenom", StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedVenom},
		{"PoisonSaveFailedKillingVenom", StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedKillingVenom},
		{"PoisonSaveFailedParalyzingVenom", StaticLq::CombatEnum::CombatantValue::TPoisonSaveFailedParalyzingVenom},

		{"PhysicalStrength", StaticLq::CombatEnum::CombatantValue::TPhysicalStrength},
		{"Stamina", StaticLq::CombatEnum::CombatantValue::TStamina},
		{"Agility", StaticLq::CombatEnum::CombatantValue::TAgility},
		{"ManualDexterity", StaticLq::CombatEnum::CombatantValue::TManualDexterity},
		{"Perception", StaticLq::CombatEnum::CombatantValue::TPerception},
		{"Willpower", StaticLq::CombatEnum::CombatantValue::TWillpower},
		{"Faith", StaticLq::CombatEnum::CombatantValue::TFaith},

		{"RawPhysicalStrength", StaticLq::CombatEnum::CombatantValue::TRawPhysicalStrength},
		{"RawStamina", StaticLq::CombatEnum::CombatantValue::TRawStamina},
		{"RawAgility", StaticLq::CombatEnum::CombatantValue::TRawAgility},
		{"RawManualDexterity", StaticLq::CombatEnum::CombatantValue::TRawManualDexterity},
		{"RawPerception", StaticLq::CombatEnum::CombatantValue::TRawPerception},
		{"RawWillpower", StaticLq::CombatEnum::CombatantValue::TRawWillpower},
		{"RawFaith", StaticLq::CombatEnum::CombatantValue::TRawFaith},

		{"BuffPhysicalStrength", StaticLq::CombatEnum::CombatantValue::TBuffPhysicalStrength},
		{"BuffStamina", StaticLq::CombatEnum::CombatantValue::TBuffStamina},
		{"BuffAgility", StaticLq::CombatEnum::CombatantValue::TBuffAgility},
		{"BuffManualDexterity", StaticLq::CombatEnum::CombatantValue::TBuffManualDexterity},
		{"BuffPerception", StaticLq::CombatEnum::CombatantValue::TBuffPerception},
		{"BuffWillpower", StaticLq::CombatEnum::CombatantValue::TBuffWillpower},
		{"BuffFaith", StaticLq::CombatEnum::CombatantValue::TBuffFaith},

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


