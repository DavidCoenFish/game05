#pragma once

#include "static_lq/roll_data.h"
#include "static_lq/bestiary/monster_attack_data.h"

namespace StaticLq
{
struct MonsterVariationData
{
public:
	// could be locale key
	std::string _display_name = {};

	int32_t _combat_level = 0;
	int32_t _attack_bonus = 0;
	int32_t _defence = 0;
	RollData _damage_tolerance = {};
	int32_t  _experence_point_value = 0;
	std::vector<MonsterAttackData> _array_attack_data = {};

};
}
