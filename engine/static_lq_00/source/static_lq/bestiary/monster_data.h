#pragma once

#include "static_lq/bestiary/bestiary_enum.h"
#include "static_lq/bestiary/monster_attribute_data.h"
#include "static_lq/bestiary/monster_variation_data.h"
#include "static_lq/bestiary/special_characteristic_data.h"

namespace StaticLq
{
struct MonsterData
{
public:
	// locale key
	std::string _species = {};

	BestiaryEnum::Habitat _habitat = BestiaryEnum::Habitat::TDefault;
	BestiaryEnum::Lifestyle _lifestyle = BestiaryEnum::Lifestyle::TDefault;
	BestiaryEnum::WealthType  _wealth_type = BestiaryEnum::WealthType::TDefault;
	BestiaryEnum::Alignment _alignment = BestiaryEnum::Alignment::TDefault;
	BestiaryEnum::Cunning _cunning = BestiaryEnum::Cunning::TDefault;
	int _speed = 0;
	BestiaryEnum::Strength _strength = BestiaryEnum::Strength::TDefault;
	BestiaryEnum::Size _size = BestiaryEnum::Size::TDefault;
	std::vector<SpecialCharacteristicData>_array_special_characteristic;
	int _recovery_time = 0;
	int _absorption = 0;
	int _surprise_adjust = 0;
	bool _is_living_creature = false;
	MonsterAttributeData _attribute_data;
	std::vector<MonsterVariationData> _array_variation;

};
}
/*
	std::vector<BestiaryEnum::Habitat>_habitat = {};
	std::vector<BestiaryEnum::Lifestyle> _lifestyle = {};
	std::vector<BestiaryEnum::WealthType> _wealth_type = {};
	BestiaryEnum::Alignment _alignment = BestiaryEnum::Alignment::TDefault;
	BestiaryEnum::Cunning _cunning = BestiaryEnum::Cunning::TDefault;
	int _speed = 0;
	BestiaryEnum::Strength _strength = BestiaryEnum::Strength::TDefault;
	BestiaryEnum::Size _size = BestiaryEnum::Size::TDefault;
	//std::vector<BestiaryEnum::SpecialCharacteristics> _special_characteristic = {};
	//std::vector<SpecialCharacteristicData>_array_special_characteristic;
	int _recovery_time = 0;
	int _absorption = 0;
	int _surprise_adjust = 0;
	//std::vector<MonsterAttackData> _array_attack;
	//_arrayAttackName : [ "mandibles" ],

	//int _physical_strength = 0;
	//int _stamina = 0;
	//int _agility = 0;
	//int _manual_dexterity = 0;
	//int _perception = 0;
	//int _willpower = 0;
	//int _faith = 0;

	int _combat_level = 0;

	int _attack_bonus = 0;
	int _defence = 0;
	RollData _damage_tolerance;
	int _experence_points = 0;

	bool _effected_by_severity_damage = false;
	bool _effected_by_faith_damage = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	MonsterData,
	_species,
	_inheritance_parent,

	_habitat,
	_lifestyle,
	_wealth_type,
	_alignment,
	_cunning,
	_speed,
	_strength,
	_size,
	_special_characteristic,
	_recovery_time,
	_absorption,
	_surpriseAdjust,
	//_arrayAttackName,

	_physical_strength,
	_stamina,
	_agility,
	_manual_dexterity,
	_perception,
	_willpower,
	_faith,

	_effected_by_severity_damage,
	_effected_by_faith_damage
	);
}
*/
