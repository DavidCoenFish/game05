#pragma once

namespace StaticLq
{
class ICombatant;
class ICombatOutput;

class CombatDamage
{
public:
	// todo: faith damage, need aligment of trigger, need "is magical damage" flag?
	explicit CombatDamage(
		const int32_t in_physical_damage_delta = 0,
		const int32_t in_severity_damage_delta = 0,
		const int32_t in_fatigue_damage_delta = 0,
		const int32_t in_paralyzation_damage_delta = 0
		);
	~CombatDamage();

	void Apply(
		ICombatant& in_target,
		ICombatOutput* in_output
		);

private:
	int32_t _physical_damage_delta = 0;
	int32_t _severity_damage_delta = 0;
	int32_t _fatigue_damage_delta = 0;
	int32_t _paralyzation_damage_delta = 0;

	/// todo, may need tooltips to document how each value was generated
	// std::shared_ptr<TooltipData> _physical_damage_tooltip;
	// std::shared_ptr<TooltipData> _severity_damage_tooltip;
	// std::shared_ptr<TooltipData> _fatigue_damage_tooltip;
	// std::shared_ptr<TooltipData> _paralyzation_damage_tooltip;

};
}
