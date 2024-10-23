#pragma once

#include "common/util/enum_soft_bind.h"

namespace StaticLq
{
namespace BestiaryEnum
{
enum class MonsterAttribute
{
	TDefault = 0,

	TNameKey, // key for name system to generate name with
	TSpeciesName, // "giant spider", "giant ant"
	TVariationName, // "average", "warrior", "queen", "worker"
	TDescription,

	TCombatLevel,
	TAttackBonus,
	TDefence,
	TDamageToleranceRollData,
	TExperencePointValue,

	TSpeed,
	TRecoveryTime,
	TAbsorption,
	TSurpriseAdjust,

	TMonsterType,
	THabitat,
	TLifestyle,
	TWealthType,
	TAlignment,
	TCunning,
	TStrength,
	TSize,
	TSpecialCharacteristicData,
	TMonsterAttackData,

	TAttributePhysicalStrength,
	TAttributeStamina,
	TAttributeAgility,
	TAttributeManualDexterity,
	TAttributePerception,
	TAttributeWillpower,
	TAttributeFaith,

	TIsEffectedByPoison,
	TIsEffectedBySeverityDamage,
	TIsEffectedByPysicalDamage,
	TIsEffectedByMagicDamage,
	TIsEffectedByFaithDamage,

};

enum class MonsterType
{
	TDefault = 0,
	IsAlive, // effected by severity damage, poision
	//TMagical
	//TFaith
	//TUndead
};

enum class Habitat
{
	TDefault = 0,
	TForest = 1 << 0,
	TSubterranean = 1 << 1,
	TDesert = 1 << 2,

	TEverywhere = -1
};
NLOHMANN_JSON_SERIALIZE_ENUM( Habitat, {
	{Habitat::TDefault, nullptr},
	{Habitat::TEverywhere, "everywhere"},
	{Habitat::TForest, "forest"},
	{Habitat::TSubterranean, "subterranean"},
	{Habitat::TEverywhere, "everywhere"},
});

enum class Lifestyle
{
	TDefault = 0,
	TComunal = 1 << 0,
	TInstinctive = 1 << 1,
	TInsect = 1 << 2,
	TArachnid = 1 << 3,
};
NLOHMANN_JSON_SERIALIZE_ENUM( Lifestyle, {
	{Lifestyle::TDefault, nullptr},
	{Lifestyle::TComunal, "comunal"},
	{Lifestyle::TInstinctive, "instinctive"},
	{Lifestyle::TInsect, "insect"},
	{Lifestyle::TArachnid, "arachnid"},
});

//_wealthType : LegendaryQuest.Bestiary.Enum.s_wealth.TMineral,

enum class WealthType
{
	TDefault = 0,
	TMineral = 1 << 0,
	TIncidental = 1 << 1
};
NLOHMANN_JSON_SERIALIZE_ENUM( WealthType, {
	{WealthType::TDefault, nullptr},
	{WealthType::TMineral, "mineral"},
	{WealthType::TIncidental, "incidental"},
});

//_alignment :  LegendaryQuest.Bestiary.Enum.s_alignment.TNeutral,
enum class Alignment
{
	TDefault = 0,
	TNeutral,
	TGood,
	TEvil,
};
NLOHMANN_JSON_SERIALIZE_ENUM( Alignment, {
	{Alignment::TDefault, nullptr},
	{Alignment::TNeutral, "neutral"},
	{Alignment::TGood, "good"},
	{Alignment::TEvil, "evil"},
});

//_cunning :  LegendaryQuest.Bestiary.Enum.s_cunning.TLow,
enum class Cunning
{
	TDefault = 0,
	TLow,
};
NLOHMANN_JSON_SERIALIZE_ENUM( Cunning, {
	{Cunning::TDefault, nullptr},
	{Cunning::TLow, "low"},
});


//_strength : LegendaryQuest.Bestiary.Enum.s_strength.THigh,
enum class Strength
{
	TDefault = 0,
	TBelowAverage,
	THigh,
};
NLOHMANN_JSON_SERIALIZE_ENUM( Strength, {
	{Strength::TDefault, nullptr},
	{Strength::TBelowAverage, "below_average"},
	{Strength::THigh, "high"},
});


//_size : LegendaryQuest.Bestiary.Enum.s_size.TMedium,
enum class Size
{
	TDefault = 0,
	TSmall,
	TMedium,
};
NLOHMANN_JSON_SERIALIZE_ENUM( Size, {
	{Size::TDefault, nullptr},
	{Size::TSmall, "small"},
	{Size::TMedium, "medium"},
});

enum class PoisonType
{
	TDefault,
	TToxin,
	THallucinogen,
	TKillingVenom,
	TParalyzingVenom
,
};
NLOHMANN_JSON_SERIALIZE_ENUM( PoisonType, {
	{PoisonType::TDefault, nullptr},
	{PoisonType::TToxin, "toxin"},
	{PoisonType::THallucinogen, "hallucinogen"},
	{PoisonType::TKillingVenom, "killing venom"},
	{PoisonType::TParalyzingVenom, "paralyzing venom"},
});

}
}
