#pragma once

#include "static_lq/bestiary/bestiary_enum.h"

class LocaleSystem;
class ITooltip;
enum class LocaleISO_639_1;

/*
so, do we return a localisation key for the display name, or deal with localisation internally (via DAG?)

*/
namespace StaticLq
{
class ICombatAction;
class ICombatOutput;
class ICombatEffect;
class RandomSequence;
class CombatTime;

namespace CombatEnum
{
enum class CombatantValue;
}

class ICombatant
{
public:
	static const int32_t MakeNewId();

	virtual ~ICombatant(){}
	virtual const int32_t GetId() const = 0;

	/// if these values get sourced from a dag node collection, it's GetValue is not const
	/// if we end up with a lot of strings, replace with GetValueString(enum)?
	virtual const std::shared_ptr<ITooltip> GetSelfTooltip(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale) = 0;
	virtual const int32_t GetValue(const CombatEnum::CombatantValue in_key) = 0;
	virtual const std::shared_ptr<ITooltip> GetTooltip(const CombatEnum::CombatantValue in_key, const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale) = 0;

	virtual void SetValue(const CombatEnum::CombatantValue in_key, const int32_t in_value) = 0;

	/// at the start of a segment, allow effects to trigger
	virtual void TriggerEffects(
		const CombatTime& in_combat_time,
		RandomSequence& in_out_random_sequence,
		ICombatOutput* in_output
		) = 0;

	virtual void GatherAction(
		std::vector<std::shared_ptr<ICombatAction>>& out_actions,
		RandomSequence& in_out_random_sequence,
		const CombatTime& in_combat_time,
		const std::vector<std::shared_ptr<ICombatant>>& in_team_mellee,
		const std::vector<std::shared_ptr<ICombatant>>& in_team_range,
		const std::vector<std::shared_ptr<ICombatant>>& in_opponent_mellee,
		const std::vector<std::shared_ptr<ICombatant>>& in_opponent_range
		) = 0;

	/// apply damage, apply poison effect?
	virtual void ApplyEffect(
		RandomSequence& in_out_random_sequence,
		const std::shared_ptr<ICombatEffect>& in_effect,
		const CombatTime& in_combat_time,
		ICombatOutput* in_output
		) = 0;

	/// some effects are delayed, and want to keep combat going till they take effect, is. poison
	virtual const bool HasPostCombatEffect() const = 0;

	/// could pass a CombatDamage rather than keep an adding to the list or param for more types of damage
	/// but would then have a very couppled [ApplyDamageDelta-CombatDamage], else have CombatDamage know
	/// how to apply damage to an ICombatant.
	/// you could use SetValue, but there is some logic to not allow negative value totals for damage
	/// positive to do damage, negative to heal
	/// assume absorption already applied, assume already checked for being subseptable to severity damage and it being added to pyhsical damage
	virtual void ApplyDamageDelta(
		const int32_t in_physical_damage_delta,
		const int32_t in_fatigue_damage_delta,
		const int32_t in_paralyzation_damage_delta
		// take output and report if status [alive, paralized] changes with damage applied
		) = 0;

	virtual void AddBuff(StaticLq::CombatEnum::CombatantValue in_value_key, const int32_t in_delta) = 0;

};

}
