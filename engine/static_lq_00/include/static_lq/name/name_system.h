#pragma once

class LocaleSystem;
class NameSystemImplementation;
enum class LocaleISO_639_1;

namespace StaticLq
{
class INameSystemGenerator;

class NameSystem
{
public:
	static const std::string GetKeySide();
	static const std::string GetKeyGiantAnt();
	static const std::string GetKeyGiantSpider();

	// could be static, but need a ref to the list of generated names, which may be under the NameSystem
	static void RegisterLocaleSystem(const std::shared_ptr<NameSystem>& in_name_system,  LocaleSystem& in_locale_system);

	NameSystem();
	~NameSystem();

	void AddGenerator(const std::string& in_key, const std::shared_ptr<INameSystemGenerator>& in_generator);

	// return locale key, add locale value for key to locale system for the current locale, keep record of used generation data to regenerate for locale system on locale change via provider
	const std::string GenerateName(const std::string& in_key, const int in_seed, LocaleSystem& in_locale_system);

	void AddAllValuesToLocaleSystem(LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale);

private:
	std::unique_ptr< NameSystemImplementation > _implementation;
};
}
