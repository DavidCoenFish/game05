#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/monster_data.h"


std::shared_ptr<StaticLq::MonsterData> StaticLq::MonsterData::FactoryOverlay(
	const std::shared_ptr<MonsterData>& in_base,
	const std::shared_ptr<MonsterData>& in_overlay
	)
{
	auto result = std::make_shared<StaticLq::MonsterData>();

	for (const auto& item : in_base->MapAttibute)
	{
		result->MapAttibute[item.first] = item.second;
	}
	for (const auto& item : in_overlay->MapAttibute)
	{
		result->MapAttibute[item.first] = item.second;
	}

	return result;
}
