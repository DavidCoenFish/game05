#pragma once

namespace StaticLq
{
struct MonsterAttributeData
{
public:
	std::optional<int32_t> _physical_strength;
	std::optional<int32_t> _stamina;
	std::optional<int32_t> _agility;
	std::optional<int32_t> _manual_dexterity;
	std::optional<int32_t> _perception;
	std::optional<int32_t> _willpower;
	std::optional<int32_t> _faith;
};
}
