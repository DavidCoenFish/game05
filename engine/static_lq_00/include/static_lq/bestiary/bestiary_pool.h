#pragma once

class BestiaryPoolImplementation;
class LocaleSystem;

namespace StaticLq
{
class ICombatant;
class NameSystem;
struct MonsterData;

class BestiaryPool
{
public:
	/// somewhere for the test data locale data to be specified, 
	/// normal usage may be to get locale data from disk, so please don't just keep on putting everything in here
	static void RegisterLocaleSystem(LocaleSystem& in_locale_system);

	static std::shared_ptr<BestiaryPool> FactoryWithTestData();
	static const std::string GetTestDataKeyGiantAnt();
	static const std::string GetTestDataKeyGiantSpider();

	//static std::shared_ptr<BestiaryPool> FactoryFromFile(const std::filesystem::path& in_file_path);

	BestiaryPool();

	std::shared_ptr<ICombatant> MakeCombatant(
		const std::string& in_key,
		StaticLq::NameSystem& in_name_system, 
		LocaleSystem& in_locale_system 
		);

private:
	std::unique_ptr< BestiaryPoolImplementation > _implementation;

};
}
