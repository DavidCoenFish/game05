#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combat_output.h"

#include "common/log/log.h"
#include "common/locale/locale_system.h"
#include "common/locale/i_locale_data_provider.h"
#include "common/locale/locale_string_format_map.h"
#include "common/tooltip/tooltip_data.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_side.h"

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

	class LocaleDataProvider : public ILocaleDataProvider
	{
	private:
		/// for debug, 
		void Populate(LocaleSystem& in_out_locale_system, const LocaleISO_639_1 in_locale) const override
		{
			in_locale;

			const std::vector<LocaleSystem::Data> data = {
				{s_locale_key_combat_started, "Combat started\n"},
				{s_locale_key_combatant_added, "Combatant {combatant} added to side {side}\n"},
				{s_locale_key_combatant_removed, "Combatant {combatant} removed from side {side}\n"},
				{s_locale_key_set_turn_segment, "Turn {turn} Segment {segment}\n"},
				{s_locale_key_combat_ended, "Combat ended\n"},
				{s_locale_key_side_victory, "Victory for side {side}\n"},
				{s_locale_key_combatant_damage, "{combatant} took {physical_damage} physical damage, {fatigue_damage} fatigue damage, {paralyzation_damage} paralyzation damage\n"},
				{s_locale_key_set_mellee_initiative, "{combatant} set mellee inititive {value}\n"},
				{s_locale_key_attempt_mellee_attack, "{combatant} tries to {attack} {target} with attack roll {attack_roll} attack bonus {attack_bonus} against defence {defence}. is hit {hit}\n"},
				};

			in_out_locale_system.Append(in_locale, data);
		}
	};
}


void StaticLq::SimpleCombatOutput::RegisterLocaleSystem(LocaleSystem& in_locale_system)
{
	in_locale_system.RegisterProvider(std::make_shared<LocaleDataProvider>());
}

StaticLq::SimpleCombatOutput::SimpleCombatOutput(const FCallback& in_log, const std::shared_ptr<LocaleSystem>& in_locale_system)
: _log(in_log)
, _locale_system(in_locale_system)
{
	// nop
}

void StaticLq::SimpleCombatOutput::SetCombatStart()
{
	_log(_locale_system->GetValue(s_locale_key_combat_started));
}

void StaticLq::SimpleCombatOutput::CombatantAdded(ICombatant& combatant, ICombatSide& side)
{
	std::map<std::string, std::string> data_map = {
		{ "combatant", _locale_system->GetValue(combatant.GetDisplayName()) },
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
		{ "combatant", _locale_system->GetValue(combatant.GetDisplayName()) },
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
	std::map<std::string, std::string> data_map = {
		{ "combatant", _locale_system->GetValue(in_combatant.GetDisplayName()) },
		{ "value", std::to_string(in_value) }
	};
	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_set_mellee_initiative,
		format_map
		);

	_log(format_map.GetResult());

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
		{ "combatant", _locale_system->GetValue(in_combatant_performing_action->GetDisplayName()) },
		{ "target", _locale_system->GetValue(in_combatant_being_hit->GetDisplayName()) },
		{ "attack", _locale_system->GetValue(in_attack) },
		{ "attack_roll", std::to_string(in_attack_roll) },
		{ "attack_bonus", std::to_string(in_attack_bonus) },
		{ "defence", std::to_string(in_defence) },
		{ "hit", std::to_string(in_hit) }
	};
	LocaleStringFormatMap format_map(data_map);

	_locale_system->GetValueFormatted(
		s_locale_key_attempt_mellee_attack,
		format_map
		);

	_log(format_map.GetResult());
}

void StaticLq::SimpleCombatOutput::CombatantDamage(
	ICombatant& in_combatant_receive, 
	ICombatant* const in_cause_damage, 
	int32_t in_physical_damage_delta,
	int32_t in_fatigue_damage_delta,
	int32_t in_paralyzation_damage_delta
	)
{
	in_cause_damage;
	std::map<std::string, std::string> data_map = {
		{ "combatant", _locale_system->GetValue(in_combatant_receive.GetDisplayName()) },
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
