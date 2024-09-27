#pragma once

class LocaleSystem;
class NameSystemImplementation;
enum class LocaleISO_639_1;

namespace static_lq
{
/// data to generate a section of a name
struct NameSystemGeneratorData
{
    explicit NameSystemGeneratorData(
        const std::string in_prepend_first = "",
        const std::map<LocaleISO_639_1, std::vector<std::string>>& in_pool = {},
        const int in_repeat_pool_count = 0,
        const int in_post_digit_range = 0
        );

    /// if not empty, put this string on the front of the section of the name
    std::string prepend_first; 
    /// defaults to LocaleISO_639_1::Default if no locale match
    std::map<LocaleISO_639_1, std::vector<std::string>> pool; 
    // how many time to sample pool
    int repeat_pool_count; 
    // zero or negative, do not use, else postpend section with a (random_num()) % post_digit_range
    int post_digit_range;
};

}
