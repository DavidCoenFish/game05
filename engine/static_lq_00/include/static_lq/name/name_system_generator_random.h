#pragma once

#include "static_lq/name/i_name_system_generator.h"
#include "static_lq/name/name_system_generator_random_data.h"

enum class LocaleISO_639_1;

namespace static_lq
{
class NameSystemGeneratorRandom : public INameSystemGenerator
{
public:
    // this was for unit test, normal use wants to load generators from data? or move statics to another file?
    static const std::string GetKeySide();
    static std::shared_ptr<INameSystemGenerator> FactoryGeneratorSide();
    static const std::string GetKeyGiantAnt();
    static std::shared_ptr<INameSystemGenerator> FactoryGeneratorGiantAnt();
    static const std::string GetKeyGiantSpider();
    static std::shared_ptr<INameSystemGenerator> FactoryGeneratorGiantSpider();

    NameSystemGeneratorRandom(const std::vector<NameSystemGeneratorData>& in_generator_data);

private:
    const std::string GenerateName(const int in_seed, const LocaleISO_639_1 locale) const override;

private:
    std::vector<NameSystemGeneratorData> _generator_data;

};
}
