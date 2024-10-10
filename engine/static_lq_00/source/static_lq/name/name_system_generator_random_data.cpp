#include "static_lq/static_lq_pch.h"
#include "static_lq/name/name_system_generator_random_data.h"

StaticLq::NameSystemGeneratorData::NameSystemGeneratorData(
	const std::string in_prepend_first,
	const std::map<LocaleISO_639_1, std::vector<std::string>>& in_pool,
	const int in_repeat_pool_count_min,
	const int in_repeat_pool_count_max,
	const int in_post_digit_range
	)
	: prepend_first(in_prepend_first)
	, pool(in_pool)
	, repeat_pool_count_min(in_repeat_pool_count_min)
	, repeat_pool_count_max(in_repeat_pool_count_max)
	, post_digit_range(in_post_digit_range)
{
	// nop
}
