#include "static_lq/static_lq_pch.h"
#include "static_lq/name/name_system_generator_random.h"

#include "common/locale/locale_enum.h"
#include "static_lq/random_sequence.h"

std::shared_ptr<StaticLq::INameSystemGenerator> StaticLq::NameSystemGeneratorRandom::FactoryGeneratorSide()
{
    std::vector<NameSystemGeneratorData> generator_data;
    {
        NameSystemGeneratorData data;
        data.pool[LocaleISO_639_1::Default] = std::vector<std::string>({
            "cold",
            "warm",
            "crusty",
            "smooth",
            "alkward",
            "shy",
            "loud",
            "red",
            "green",
            "blue",
            "orange",
            "pink"
        });
        data.repeat_pool_count = 1;
        generator_data.push_back(data);
    }
    {
        NameSystemGeneratorData data;
        data.prepend_first = " ";
        data.pool[LocaleISO_639_1::Default] = std::vector<std::string>({
            "summer",
            "winter",
            "autumn",
            "spring",
            "new",
            "old",
            "cheap",
            "expensive",
            "discount",
            "budget"
        });
        data.repeat_pool_count = 1;
        generator_data.push_back(data);
    }
    {
        NameSystemGeneratorData data;
        data.prepend_first = " ";
        data.pool[LocaleISO_639_1::Default] = std::vector<std::string>({
            "breeze",
            "fish",
            "moose",
            "people",
            "trees",
            "moon",
            "sun",
            "stars",
            "morning",
            "afternoon",
            "night",
            "day",
        });
        data.repeat_pool_count = 1;
        generator_data.push_back(data);
    }
    {
        NameSystemGeneratorData data;
        data.prepend_first = " ";
        data.post_digit_range = 1000;
        generator_data.push_back(data);
    }

    return std::make_shared<NameSystemGeneratorRandom>(generator_data);
}

std::shared_ptr<StaticLq::INameSystemGenerator> StaticLq::NameSystemGeneratorRandom::FactoryGeneratorGiantAnt()
{
    std::vector<NameSystemGeneratorData> generator_data;
    {
        NameSystemGeneratorData data;
        data.pool[LocaleISO_639_1::Default] = std::vector<std::string>({
            "ti",
            "to",
            "TI",
            "TO",
        });
        data.repeat_pool_count = 8;
        generator_data.push_back(data);
    }
    return std::make_shared<NameSystemGeneratorRandom>(generator_data);
}

std::shared_ptr<StaticLq::INameSystemGenerator> StaticLq::NameSystemGeneratorRandom::FactoryGeneratorGiantSpider()
{
    std::vector<NameSystemGeneratorData> generator_data;
    {
        {
            NameSystemGeneratorData data;
            data.prepend_first = "x";
            data.pool[LocaleISO_639_1::Default] = std::vector<std::string>({
                "I",
                "l"
            });
            data.repeat_pool_count = 32;
            generator_data.push_back(data);
        }
    }
    return std::make_shared<NameSystemGeneratorRandom>(generator_data);
}

StaticLq::NameSystemGeneratorRandom::NameSystemGeneratorRandom(const std::vector<NameSystemGeneratorData>& in_generator_data)
: _generator_data(in_generator_data)
{
    // nop
}

const std::string StaticLq::NameSystemGeneratorRandom::GenerateName(const int in_seed, const LocaleISO_639_1 in_locale) const
{
    std::string result;
    RandomSequence random_sequence(in_seed);
    //uint32_t value = random_sequence.Generate();
    for (const auto& segment : _generator_data)
    {
        result += segment.prepend_first;
        if (0 < segment.pool.size())
        {
            auto found = segment.pool.find(in_locale);
            if (found == segment.pool.end())
            {
                found = segment.pool.find(LocaleISO_639_1::Default);
            }

            if (found != segment.pool.end())
            {
                for (int index = 0; index < segment.repeat_pool_count; ++index)
                {
                    const auto& segment_pool = found->second;
                    const int sub_index = random_sequence.Generate() % segment_pool.size();
                    result += segment_pool[sub_index];
                }
            }
        }
        if (0 < segment.post_digit_range)
        {
            const int digit = random_sequence.Generate() % segment.post_digit_range;
            result += std::to_string(digit);
        }
    }
    return result;
}

