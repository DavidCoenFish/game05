#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/effect/combat_effect_poison.h"

#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/effect/combat_effect_damage.h"
#include "static_lq/combat/effect/combat_effect_stat_change.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_output.h"
#include "static_lq/random_sequence.h"

namespace
{
/*
Toxin
Risk Factor Effects
Minimally Hazardous 1d8 Damage
Moderately Hazardous 1d8 Damage
Dangerous 1d8 Damage, Nausea
Deadly 1d8 Damage, Lethargy
*/
const int32_t ApplyToxin(int32_t& out_physical_damage_dice_count, int32_t& out_paralyzation_damage_dice_count, const int32_t in_current_dose, const int32_t in_risk_factor, StaticLq::ICombatant& in_target, const StaticLq::CombatTime& in_time, StaticLq::RandomSequence& in_out_random_sequence, StaticLq::ICombatOutput* in_output)
{
	in_output;
	const int32_t result = in_current_dose + in_risk_factor;
	int32_t trace = in_current_dose;
	while (trace < result)
	{
		const int32_t index = trace % 4;
		trace += 1;
		out_physical_damage_dice_count += 1;
		if (2 == index)
		{
			// todo: Nausea
			//in_target.ApplyEffect(
			//	in_out_random_sequence,
			//	std::make_shared<StaticLq::CombatEffectStatus>(Nausea),
			//	in_time,
			//	in_output
			//	);
		}
		else if (3 == index)
		{
			// todo: Lethargy
			//in_target.ApplyEffect(
			//	in_out_random_sequence,
			//	std::make_shared<StaticLq::CombatEffectStatus>(Lethargy),
			//	in_time,
			//	in_output
			//	);
		}
	}
	return result;
}

/*
Hallucinogens
Risk Factor Effects
Minimally Hazardous Lethargy
Moderately Hazardous Fogged Perception
Dangerous Delirium, Panic
Deadly Exhaustion,
Permanent random form of
Insanity

effects don't wrap
*/
const int32_t ApplyHallucinogen(int32_t& out_physical_damage_dice_count, int32_t& out_paralyzation_damage_dice_count, const int32_t in_current_dose, const int32_t in_risk_factor, StaticLq::ICombatant& in_target, const StaticLq::CombatTime& in_time, StaticLq::RandomSequence& in_out_random_sequence, StaticLq::ICombatOutput* in_output)
{
	in_output;
	const int32_t result = in_current_dose + in_risk_factor;
	if ((in_current_dose < 1) && (1 <= result))
	{
		// todo: Lethargy
		//in_target.ApplyEffect(
		//	in_out_random_sequence,
		//	std::make_shared<StaticLq::CombatEffectStatus>(Lethargy),
		//	in_time,
		//	in_output
		//	);
	}
	if ((in_current_dose < 2) && (2 <= result))
	{
		// todo: Fogged Perception
		//in_target.ApplyEffect(
		//	in_out_random_sequence,
		//	std::make_shared<StaticLq::CombatEffectStatus>(Fogged Perception),
		//	in_time,
		//	in_output
		//	);
	}
	if ((in_current_dose < 3) && (3 <= result))
	{
		// todo: Delirium
		//in_target.ApplyEffect(
		//	in_out_random_sequence,
		//	std::make_shared<StaticLq::CombatEffectStatus>(Delirium),
		//	in_time,
		//	in_output
		//	);
		// todo: Panic
		//in_target.ApplyEffect(
		//	in_out_random_sequence,
		//	std::make_shared<StaticLq::CombatEffectStatus>(Panic),
		//	in_time,
		//	in_output
		//	);
	}
	if ((in_current_dose < 4) && (4 <= result))
	{
		// todo: Permanent random form of Insanity
	}
	return result;
}

/*
Killing Venom
Risk Factor Effects
Minimally Hazardous 1d8 Damage
Moderately Hazardous 1d8 Damage
Dangerous 1d8 Damage, -1 ST
Deadly 1d8 Damage, -1 PS
*/
const int32_t ApplyKillingVenom(int32_t& out_physical_damage_dice_count, int32_t& out_paralyzation_damage_dice_count, const int32_t in_current_dose, const int32_t in_risk_factor, StaticLq::ICombatant& in_target, const StaticLq::CombatTime& in_time, StaticLq::RandomSequence& in_out_random_sequence, StaticLq::ICombatOutput* in_output)
{
	in_output;
	const int32_t result = in_current_dose + in_risk_factor;
	int32_t trace = in_current_dose;
	while (trace < result)
	{
		const int32_t index = trace % 4;
		trace += 1;
		out_physical_damage_dice_count += 1;
		if (2 == index)
		{
			in_target.ApplyEffect(
				in_out_random_sequence,
				std::make_shared<StaticLq::CombatEffectStatChange>(StaticLq::CombatEnum::CombatantValue::TBuffStamina, -1),
				in_time,
				in_output
				);
		}
		else if (3 == index)
		{
			in_target.ApplyEffect(
				in_out_random_sequence,
				std::make_shared<StaticLq::CombatEffectStatChange>(StaticLq::CombatEnum::CombatantValue::TBuffPhysicalStrength, -1),
				in_time,
				in_output
				);
		}
	}
	return result;
}

/*
Paralyzing Venom
Risk Factor Effects
Minimally Hazardous 1d8 Damage
Moderately Hazardous 1d8 Damage
Dangerous 1d8 Damage, -1 PC
Deadly 1d8 Damage, -1 AG
*/
const int32_t ApplyParalyzingVenom(int32_t& out_physical_damage_dice_count, int32_t& out_paralyzation_damage_dice_count, const int32_t in_current_dose, const int32_t in_risk_factor, StaticLq::ICombatant& in_target, const StaticLq::CombatTime& in_time, StaticLq::RandomSequence& in_out_random_sequence, StaticLq::ICombatOutput* in_output)
{
	in_output;
	const int32_t result = in_current_dose + in_risk_factor;
	int32_t trace = in_current_dose;
	while (trace < result)
	{
		const int32_t index = trace % 4;
		trace += 1;
		out_paralyzation_damage_dice_count += 1;
		if (2 == index)
		{
			in_target.ApplyEffect(
				in_out_random_sequence,
				std::make_shared<StaticLq::CombatEffectStatChange>(StaticLq::CombatEnum::CombatantValue::TBuffPerception, -1),
				in_time,
				in_output
				);
		}
		else if (3 == index)
		{
			in_target.ApplyEffect(
				in_out_random_sequence,
				std::make_shared<StaticLq::CombatEffectStatChange>(StaticLq::CombatEnum::CombatantValue::TBuffAgility, -1),
				in_time,
				in_output
				);
		}
	}
	return result;
}

}

StaticLq::CombatEffectPoison::CombatEffectPoison(
	const CombatTime& in_trigger_time,
	const BestiaryEnum::PoisonType in_type,
	const int32_t in_risk_factor
	)
	: _trigger_time(in_trigger_time)
	, _type(in_type)
	, _risk_factor(in_risk_factor)
{
	// nop
}

const bool StaticLq::CombatEffectPoison::Apply(
	RandomSequence& in_out_random_sequence,
	const CombatTime& in_combat_time,
	ICombatant& in_target,
	ICombatOutput* in_output
	)
{
	if (false == CombatTime::Equivalent(in_combat_time, _trigger_time))
	{
		// effect keeps applying till hitting trigger
		return true;
	}

	//add the dose
	CombatEnum::CombatantValue dose_enum = CombatEnum::CombatantValue::TDefault;
	switch (_type)
	{
	default:
		break;
	case BestiaryEnum::PoisonType::THallucinogen:
		dose_enum = CombatEnum::CombatantValue::TPoisonDoseCountHallucinogen;
		break;
	case BestiaryEnum::PoisonType::TKillingVenom:
		dose_enum = CombatEnum::CombatantValue::TPoisonDoseCountKillingVenom;
		break;
	case BestiaryEnum::PoisonType::TParalyzingVenom:
		dose_enum = CombatEnum::CombatantValue::TPoisonDoseCountVenom;
		break;
	case BestiaryEnum::PoisonType::TToxin:
		dose_enum = CombatEnum::CombatantValue::TPoisonDoseCountToxin;
		break;
	}

	const int32_t current_dose = in_target.GetValue(dose_enum);
	int32_t new_dose = current_dose;

	//apply damage, status effects
	int32_t physical_damage_dice_count = 0;
	int32_t paralyzation_damage_dice_count = 0;
	switch (_type)
	{
	default:
		break;
	case BestiaryEnum::PoisonType::THallucinogen:
		new_dose = ApplyHallucinogen(physical_damage_dice_count, paralyzation_damage_dice_count, current_dose, _risk_factor, in_target, in_combat_time, in_out_random_sequence, in_output);
		break;
	case BestiaryEnum::PoisonType::TKillingVenom:
		new_dose = ApplyKillingVenom(physical_damage_dice_count, paralyzation_damage_dice_count, current_dose, _risk_factor, in_target, in_combat_time, in_out_random_sequence, in_output);
		break;
	case BestiaryEnum::PoisonType::TParalyzingVenom:
		new_dose = ApplyParalyzingVenom(physical_damage_dice_count, paralyzation_damage_dice_count, current_dose, _risk_factor, in_target, in_combat_time, in_out_random_sequence, in_output);
		break;
	case BestiaryEnum::PoisonType::TToxin:
		new_dose = ApplyToxin(physical_damage_dice_count, paralyzation_damage_dice_count, current_dose, _risk_factor, in_target, in_combat_time, in_out_random_sequence, in_output);
		break;
	}

	in_target.SetValue(dose_enum, new_dose);

	// use random sequence to work out damage
	if ((0 < physical_damage_dice_count) || (0 < paralyzation_damage_dice_count))
	{
		int32_t physical_damage_delta = 0;
		int32_t paralyzation_damage_delta = 0;
		for (int32_t index = 0; index < physical_damage_dice_count; ++index)
		{
			physical_damage_delta += in_out_random_sequence.GenerateDice(8);
		}
		for (int32_t index = 0; index < paralyzation_damage_dice_count; ++index)
		{
			paralyzation_damage_delta += in_out_random_sequence.GenerateDice(8);
		}

		//CombatEffectDamage damage(physical_damage_delta, 0, 0, paralyzation_damage_delta);
		//damage.Apply(in_out_random_sequence, in_combat_time, in_target, in_output);

		const std::shared_ptr<ICombatEffect> combatDamage = std::make_shared<CombatEffectDamage>(physical_damage_delta, 0, paralyzation_damage_delta);
		combatDamage->Apply(
			in_out_random_sequence,
			in_combat_time,
			in_target,
			in_output
			);
	}

	return false;
}

const bool StaticLq::CombatEffectPoison::ContinuePastEndCombat() const
{
	return true;
}

