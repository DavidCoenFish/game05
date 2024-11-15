#pragma once

class LocaleSystem;
enum class LocaleISO_639_1;

namespace StaticLq
{
class INameSystemGenerator
{
public:
	static const std::string GenerateKey(const std::string& in_generator_key, const int in_seed);

	virtual ~INameSystemGenerator() {}

	virtual const std::string GenerateName(const int in_seed, const LocaleISO_639_1 locale) const = 0;
};
}
