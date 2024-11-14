#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combat_output.h"

#include "common/log/log.h"
#include "common/locale/locale_enum.h"
#include "common/locale/locale_system.h"
#include "common/locale/i_locale_data_provider.h"
#include "common/locale/locale_string_format_map.h"
#include "common/tooltip/i_tooltip.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_side.h"
#include "static_lq/combat/combat_time.h"

namespace
{
	constexpr char s_locale_key_combat_started[] = "slqsc_combat_started";
	constexpr char s_locale_key_combatant_added[] = "slqsc_combatant_added";
	constexpr char s_locale_key_combatant_removed[] = "slqsc_combatant_removed";
	constexpr char s_locale_key_set_turn_segment[] = "slqsc_set_turn_segment";
	constexpr char s_locale_key_combat_ended[] = "slqsc_combat_ended";
	constexpr char s_locale_key_side_victory[] = "slqsc_side_victory";
	constexpr char s_locale_key_combatant_damage[] = "slqsc_combatant_damage";
	constexpr char s_locale_key_set_mellee_initiative[] = "slqsc_set_mellee_initiative";
	constexpr char s_locale_key_attempt_mellee_attack[] = "slqsc_attempt_mellee_attack";
	constexpr char s_locale_key_attempt_mellee_miss[] = "slqsc_attempt_mellee_miss";
	constexpr char s_locale_key_combatant_died[] = "slqsc_combatant_died";
	constexpr char s_locale_key_combatant_poisoned[] = "slqsc_combatant_poisoned"; 
	constexpr char s_locale_key_combatant_poison_save[] = "slqsc_combatant_poison_save";

	const std::string GetTooltipText(const std::shared_ptr<ITooltip>& in_tooltip)
	{
		std::string result = {};
		if (nullptr != in_tooltip)
		{
			result = in_tooltip->GetText();
		}
		return result;
	}
}

/// for debug/ unittests. eventually, should be read from data
void StaticLq::SimpleCombatOutput::RegisterLocaleSystem(LocaleSystem& in_locale_system)
{
	//in_locale_system.RegisterProvider(std::make_shared<LocaleDataProvider>());
	const std::vector<LocaleSystem::Data> data = {
		{s_locale_key_combat_started, "Combat started\n"},
		{s_locale_key_combatant_added, "Combatant {combatant} added to side {side}\n"},
		{s_locale_key_combatant_removed, "Combatant {combatant} removed from side {side}\n"},
		{s_locale_key_set_turn_segment, "Turn {turn} Segment {segment}\n"},
		{s_locale_key_combat_ended, "Combat ended\n"},
		{s_locale_key_side_victory, "Victory for side {side}\n"},
		{s_locale_key_combatant_damage, "{combatant} took damage. {physical_damage} physical damage, {fatigue_damage} fatigue damage, {paralyzation_damage} paralyzation damage, {health_points}/{damage_tollerance}\n"},
		{s_locale_key_set_mellee_initiative, "{combatant} set mellee inititive {value}\n"},
		{s_locale_key_attempt_mellee_attack, "{combatant} attacks {target} with {attack}. defence({defence}) <= Attack roll({attack_roll}) + attack bonus({attack_bonus})\n"},
		{s_locale_key_attempt_mellee_miss, "{combatant} tries to {attack} {target} but misses. defence({defence}) <= Attack roll({attack_roll}) + attack bonus({attack_bonus})\n"},
		{s_locale_key_combatant_poison_save, "{combatant} save vrs poision, risk reduced to {risk}. threashold({threashold}) <= roll({roll}) + stamina({stamina})\n"},
		{s_locale_key_combatant_poisoned, "{combatant} poisoned! dose({risk}) at turn({turn}) segment({segment})\n"},
		{s_locale_key_combatant_died, "{combatant} died!\n"},
		};

	in_locale_system.Append(LocaleISO_639_1::Default, data);
}

StaticLq::SimpleCombatOutput::SimpleCombatOutput(const FCallback& in_log, const std::shared_ptr<LocaleSystem>& in_locale_system)
: _log(in_log)
, _locale_system(in_locale_system)
{
	// nop
}

void StaticLq::SimpleCombatOutput::DebugLog(const std::string& in_message)
{
	_log(in_message);
}

void StaticLq::SimpleCombatOutput::SetCombatStart()
{
	_log(_locale_system->GetValue(s_locale_key_combat_started));
}

void StaticLq::SimpleCombatOutput::CombatantAdded(ICombatant& combatant, ICombatSide& side)
{
	std::map<std::string, std::string> data_map = {
		{ "combatant", GetTooltipText(combatant.GetSelfTooltip(*_locale_system, _locale_system->GetLocale())) },
		{ "side", _locale_system->GetValue(side.GetDisplayName()) }
	};
	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_combatant_added,
		format_map
		);

	_log(format_map.GetResult());
}

void StaticLq::SimpleCombatOutput::CombatantRemoved(ICombatant& combatant, ICombatSide& side)
{
	std::map<std::string, std::string> data_map = {
		{ "combatant", GetTooltipText(combatant.GetSelfTooltip(*_locale_system, _locale_system->GetLocale())) },
		{ "side", _locale_system->GetValue(side.GetDisplayName()) }
	};
	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_combatant_removed,
		format_map
		);

	_log(format_map.GetResult());
}

void StaticLq::SimpleCombatOutput::SetTurnSegment(const int32_t turn, const int32_t segment)
{
	std::map<std::string, std::string> data_map = {
		{ "turn", std::to_string(turn)},
		{ "segment", std::to_string(segment)}
	};
	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_set_turn_segment,
		format_map
		);

	_log(format_map.GetResult());
}

void StaticLq::SimpleCombatOutput::CombatantSetMelleeInitiative(ICombatant& in_combatant, const int32_t in_value)
{
	in_combatant; in_value;
#if 0
	std::map<std::string, std::string> data_map = {
		{ "combatant", GetTooltipText(combatant.GetSelfTooltip(*_locale_system, _locale_system->GetLocale())) },
		{ "value", std::to_string(in_value) }
	};
	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_set_mellee_initiative,
		format_map
		);

	_log(format_map.GetResult());
#endif //#if 0
}

void StaticLq::SimpleCombatOutput::CombatantAttemptMelleeAttack(
	ICombatant* const in_combatant_performing_action, 
	ICombatant* const in_combatant_being_hit, 
	const std::string& in_attack,
	const int32_t in_attack_roll,
	const int32_t in_attack_bonus,
	const int32_t in_defence,
	const bool in_hit
	)
{
	if ((nullptr == in_combatant_performing_action) || (nullptr == in_combatant_being_hit))
	{
		return;
	}
	std::map<std::string, std::string> data_map = {
		{ "combatant", GetTooltipText(in_combatant_performing_action->GetSelfTooltip(*_locale_system, _locale_system->GetLocale())) },
		{ "target", GetTooltipText(in_combatant_being_hit->GetSelfTooltip(*_locale_system, _locale_system->GetLocale())) },
		{ "attack", _locale_system->GetValue(in_attack) },
		{ "attack_roll", std::to_string(in_attack_roll) },
		{ "attack_bonus", std::to_string(in_attack_bonus) },
		{ "defence", std::to_string(in_defence) }
	};
	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		in_hit ? s_locale_key_attempt_mellee_attack : s_locale_key_attempt_mellee_miss,
		format_map
		);

	_log(format_map.GetResult());
}

void StaticLq::SimpleCombatOutput::CombatantDamage(
	ICombatant& in_combatant_receive, 
	const int32_t in_health_points,
	const int32_t in_damage_tollerance,
	const int32_t in_physical_damage_delta,
	const int32_t in_fatigue_damage_delta,
	const int32_t in_paralyzation_damage_delta
	)
{
	std::map<std::string, std::string> data_map = {
		{ "combatant", GetTooltipText(in_combatant_receive.GetSelfTooltip(*_locale_system, _locale_system->GetLocale())) },
		{ "health_points", std::to_string(in_health_points) },
		{ "damage_tollerance", std::to_string(in_damage_tollerance) },
		{ "physical_damage", std::to_string(in_physical_damage_delta) },
		{ "fatigue_damage", std::to_string(in_fatigue_damage_delta) },
		{ "paralyzation_damage", std::to_string(in_paralyzation_damage_delta) }
	};
	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_combatant_damage,
		format_map
		);

	_log(format_map.GetResult());

}

void StaticLq::SimpleCombatOutput::CombatantDied(
	ICombatant& in_combatant
	)
{
	std::map<std::string, std::string> data_map = {
		{ "combatant", GetTooltipText(in_combatant.GetSelfTooltip(*_locale_system, _locale_system->GetLocale())) },
		};

	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_combatant_died,
		format_map
		);

	_log(format_map.GetResult());
}

void StaticLq::SimpleCombatOutput::CombatantPoisonSave(
	ICombatant& in_combatant, 
	const int32_t in_risk_after_save,
	const int32_t in_threashold,
	const int32_t in_roll, 
	const int32_t in_stamina
	)
{
	std::map<std::string, std::string> data_map = {
		{ "combatant", GetTooltipText(in_combatant.GetSelfTooltip(*_locale_system, _locale_system->GetLocale())) },
		{ "risk", std::to_string(in_risk_after_save) },
		{ "threashold", std::to_string(in_threashold) },
		{ "roll", std::to_string(in_roll) },
		{ "stamina", std::to_string(in_stamina) },
		};

	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_combatant_poison_save,
		format_map
		);

	_log(format_map.GetResult());
}

void StaticLq::SimpleCombatOutput::CombatantPoisoned(
	ICombatant& in_combatant, 
	const int32_t in_risk,
		const CombatTime& in_when
	)
{
	std::map<std::string, std::string> data_map = {
		{ "combatant", GetTooltipText(in_combatant.GetSelfTooltip(*_locale_system, _locale_system->GetLocale())) },
		{ "risk", std::to_string(in_risk) },
		{ "turn", std::to_string(in_when.GetTurn()) },
		{ "segment", std::to_string(in_when.GetSegment()) }
		};

	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_combatant_poisoned,
		format_map
		);

	_log(format_map.GetResult());
}


void StaticLq::SimpleCombatOutput::SetCombatEnd(const std::vector<std::shared_ptr<ICombatSide>>& sides_able_to_continue)
{
	_log(_locale_system->GetValue(s_locale_key_combat_ended));
	for (const auto& item : sides_able_to_continue)
	{
		std::map<std::string, std::string> data_map = {
			{ "side", _locale_system->GetValue(item->GetDisplayName()) }
		};
		LocaleStringFormatMap format_map(data_map);

		_locale_system->GetValueFormatted(
			s_locale_key_side_victory,
			format_map
			);

		_log(format_map.GetResult());
	}
}
