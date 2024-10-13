#pragma once

#include "static_lq/roll_data.h"
#include "static_lq/bestiary/bestiary_enum.h"

namespace StaticLq
{
//enum class AttackEffect
//{
//	TDefault = 0,
//	TPoison,
//};

struct MonsterAttackData
{
public:
	/// could be locale key
	std::string _display_name;

	RollData _damage;
	bool _cause_severity_damage = false;

	/// rather than have an AttackEffect enum and union of data, just have each typesafe data member, and zero there value when not appropriate
	struct EffectData
	{
		//AttackEffect _attack_effect = AttackEffect::TDefault;
		//int32_t _data0 = 0; // poison: BestiaryEnum::PoisonType
		//int32_t _data1 = 0; // poison: risk factor
		BestiaryEnum::PoisonType _poison_type = BestiaryEnum::PoisonType::TDefault;
		int32_t _poison_risk_factor = 0;
	};
	
	/// poison?
	std::vector<EffectData> _effect_trigger;

};
}
