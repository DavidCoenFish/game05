#pragma once

#include "static_lq/roll_data.h"

namespace StaticLq
{
enum class AttackEffect
{
	TDefault = 0,
	//TPoison, //
	TModerateParalyzingVenom,
};

struct MonsterAttackData
{
public:
	/// could be locale key
	std::string _display_name;

	RollData _damage;
	bool _cause_severity_damage = false;
	
	/// poison?
	std::vector<AttackEffect> _effect_trigger;

};
}
