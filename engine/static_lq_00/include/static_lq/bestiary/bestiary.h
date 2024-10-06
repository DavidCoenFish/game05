#pragma once

class LocaleSystem;

namespace StaticLq
{
class ICombatant;
class NameSystem;

class Bestiary
{
public:
	/// somewhere for locale data for the factory default combatabts to be specified
	/// normal usage may be to get locale data from disk, so please don't just keep on putting everything in here
	static void RegisterLocaleSystem(LocaleSystem& in_locale_system);

	static std::shared_ptr<ICombatant> FactoryDefaultGiantSpider(
		NameSystem& in_name_system, 
		LocaleSystem& in_locale_system 
		);
	static std::shared_ptr<ICombatant> FactoryDefaultGiantAnt(
		NameSystem& in_name_system, 
		LocaleSystem& in_locale_system
		);

};
}
