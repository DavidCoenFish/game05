#pragma once

#include "static_lq/bestiary/bestiary_enum.h"

namespace static_lq
{
class MonsterData
{
public:
    MonsterData();

	std::string _species = {};
	std::string _inheritance_parent = {};

    bestiary_enum::Habitat _habitat = bestiary_enum::Habitat::TDefault;
    std::vector<bestiary_enum::Lifestyle> _lifestyle = {};
    std::vector<bestiary_enum::WealthType> _wealthType = {};
    bestiary_enum::Alignment _alignment = bestiary_enum::Alignment::TDefault;
    bestiary_enum::Cunning _cunning = bestiary_enum::Cunning::TDefault;
	int _speed = 0;
    bestiary_enum::Strength _strength = bestiary_enum::Strength::TDefault;
    bestiary_enum::Size _size = bestiary_enum::Size::TDefault;
	//_specialCharacteristicArray : undefined,
	int _recoveryTime = 0;
	int _absorption = 0;
	int _surpriseAdjust = 0;
    //std::vector<MonsterAttackData> _arrayAttack;
	//_arrayAttackName : [ "mandibles" ],

	int _physicalStrength = 0;
	int _stamina = 0;
	int _agility = 0;
	int _manualDexterity = 0;
	int _perception = 0;
	int _willpower = 0;
	int _faith = 0;

	bool _effectedBySeverityDamage = false;
	bool _effectedByFaithDamage = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    MonsterData,
	_species,
	_inheritance_parent,

	_habitat,
	_lifestyle,
	_wealthType,
	_alignment,
	_cunning,
	_speed,
	_strength,
	_size,
	//_specialCharacteristicArray,
	_recoveryTime,
	_absorption,
	_surpriseAdjust,
	//_arrayAttackName,

	_physicalStrength,
	_stamina,
	_agility,
	_manualDexterity,
	_perception,
	_willpower,
	_faith,

	_effectedBySeverityDamage,
	_effectedByFaithDamage
    );
}
