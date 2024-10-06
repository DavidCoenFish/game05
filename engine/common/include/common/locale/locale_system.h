#pragma once

class LocaleSystemImplementation;
class ILocaleDataProvider;
class ILocaleStringFormat;
enum class LocaleISO_639_1;

class LocaleSystem
{
public:
	LocaleSystem();
	~LocaleSystem();

	/// get the value for the given locale
	const std::string GetValue(
		const LocaleISO_639_1 in_locale,
		const std::string& in_key
		) const;

	/// get the value for the current locale
	const std::string GetValue(
		const std::string& in_key
		) const;

	/// example value sourced from in_key "hello {foo} may name is {}, have a nice {day}"
	/// escape via double "hello {{world}}" => "hello {world}"
	/// "hello {world}" => "hello meow" with in_stringFormatDataSource returning "meow" for token "world"
	/// collection of result moved into ILocaleStringFormat to allow tooltip to build link data.
	/// get the formatted value for the given locale
	void GetValueFormatted(
		const LocaleISO_639_1 in_locale,
		const std::string& in_key,
		ILocaleStringFormat& in_stringFormatDataSource
		) const;

	/// get the formatted value for the current locale
	void GetValueFormatted(
		const std::string& in_key,
		ILocaleStringFormat& in_stringFormatDataSource
		) const;

	void RegisterProvider(const std::shared_ptr<ILocaleDataProvider>& in_provider);
	void SetLocaleAndPopulate(const LocaleISO_639_1 in_locale);
	const LocaleISO_639_1 GetLocale() const;

	const std::vector<LocaleISO_639_1> GatherLocale() const;

	struct Data
	{
		std::string _key;
		std::string _value;
	};
	void Append(
		const LocaleISO_639_1 in_locale,
		const std::vector<Data>& in_data
		);

	void AppendCurrent(
		const std::string& in_key,
		const std::string& in_value
		);

private:
	std::unique_ptr<LocaleSystemImplementation> _implementation;

};
