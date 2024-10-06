#include "common/common_pch.h"
#include "common/locale/locale_enum.h"

namespace
{
	struct Data
	{
		const char* const _data;
		const LocaleISO_639_1 _enum;
	};

	const char* const s_default_data = "";

	static const Data s_data[] = {
		{s_default_data, LocaleISO_639_1::Default},

		{"ab", LocaleISO_639_1::Abkhazian},
		{"aa", LocaleISO_639_1::Afar},
		{"af", LocaleISO_639_1::Afrikaans},
		{"ak", LocaleISO_639_1::Akan},
		{"sq", LocaleISO_639_1::Albanian},
		{"am", LocaleISO_639_1::Amharic},
		{"ar", LocaleISO_639_1::Arabic},
		{"an", LocaleISO_639_1::Aragonese},
		{"hy", LocaleISO_639_1::Armenian},
		{"as", LocaleISO_639_1::Assamese},
		{"av", LocaleISO_639_1::Avaric},
		{"ae", LocaleISO_639_1::Avestan},
		{"ay", LocaleISO_639_1::Aymara},
		{"az", LocaleISO_639_1::Azerbaijani},
		{"bm", LocaleISO_639_1::Bambara},
		{"ba", LocaleISO_639_1::Bashkir},
		{"eu", LocaleISO_639_1::Basque},
		{"be", LocaleISO_639_1::Belarusian},
		{"bn", LocaleISO_639_1::Bengali},
		{"bi", LocaleISO_639_1::Bislama},
		{"bs", LocaleISO_639_1::Bosnian},
		{"br", LocaleISO_639_1::Breton},
		{"bg", LocaleISO_639_1::Bulgarian},
		{"my", LocaleISO_639_1::Burmese},
		{"ca", LocaleISO_639_1::Catalan},
		{"ch", LocaleISO_639_1::Chamorro},
		{"ce", LocaleISO_639_1::Chechen},
		{"ny", LocaleISO_639_1::Chichewa},
		{"zh-cn", LocaleISO_639_1::Chinese_Peoples_Republic_of_China},
		{"zh-tw", LocaleISO_639_1::Chinese_Taiwan},
		{"cu", LocaleISO_639_1::Old_Slavonic},
		{"cv", LocaleISO_639_1::Chuvash},
		{"kw", LocaleISO_639_1::Cornish},
		{"co", LocaleISO_639_1::Corsican},
		{"cr", LocaleISO_639_1::Cree},
		{"hr", LocaleISO_639_1::Croatian},
		{"cs", LocaleISO_639_1::Czech},
		{"da", LocaleISO_639_1::Danish},
		{"dv", LocaleISO_639_1::Divehi},
		{"nl", LocaleISO_639_1::Dutch},
		{"dz", LocaleISO_639_1::Dzongkha},
		{"en", LocaleISO_639_1::English},
		{"eo", LocaleISO_639_1::Esperanto},
		{"et", LocaleISO_639_1::Estonian},
		{"ee", LocaleISO_639_1::Ewe},
		{"fo", LocaleISO_639_1::Faroese},
		{"fj", LocaleISO_639_1::Fijian},
		{"fi", LocaleISO_639_1::Finnish},
		{"fr", LocaleISO_639_1::French},
		{"fy", LocaleISO_639_1::Frisian},
		{"ff", LocaleISO_639_1::Fulah},
		{"gd", LocaleISO_639_1::Gaelic},
		{"gl", LocaleISO_639_1::Galician},
		{"lg", LocaleISO_639_1::Ganda},
		{"ka", LocaleISO_639_1::Georgian},
		{"de", LocaleISO_639_1::German},
		{"el", LocaleISO_639_1::Greek},
		{"kl", LocaleISO_639_1::Kalaallisut},
		{"gn", LocaleISO_639_1::Guarani},
		{"gu", LocaleISO_639_1::Gujarati},
		{"ht", LocaleISO_639_1::Haitian},
		{"ha", LocaleISO_639_1::Hausa},
		{"he", LocaleISO_639_1::Hebrew},
		{"hz", LocaleISO_639_1::Herero},
		{"hi", LocaleISO_639_1::Hindi},
		{"ho", LocaleISO_639_1::Hiri_Motu},
		{"hu", LocaleISO_639_1::Hungarian},
		{"is", LocaleISO_639_1::Icelandic},
		{"io", LocaleISO_639_1::Ido},
		{"ig", LocaleISO_639_1::Igbo},
		{"id", LocaleISO_639_1::Indonesian},
		{"ia", LocaleISO_639_1::Interlingua},
		{"ie", LocaleISO_639_1::Interlingue},
		{"iu", LocaleISO_639_1::Inuktitut},
		{"ik", LocaleISO_639_1::Inupiaq},
		{"ga", LocaleISO_639_1::Irish},
		{"it", LocaleISO_639_1::Italian},
		{"ja", LocaleISO_639_1::Japanese},
		{"jv", LocaleISO_639_1::Javanese},
		{"kn", LocaleISO_639_1::Kannada},
		{"kr", LocaleISO_639_1::Kanuri},
		{"ks", LocaleISO_639_1::Kashmiri},
		{"kk", LocaleISO_639_1::Kazakh},
		{"km", LocaleISO_639_1::Central_Khmer},
		{"ki", LocaleISO_639_1::Kikuyu},
		{"rw", LocaleISO_639_1::Kinyarwanda},
		{"ky", LocaleISO_639_1::Kirghiz},
		{"kv", LocaleISO_639_1::Komi},
		{"kg", LocaleISO_639_1::Kongo},
		{"ko", LocaleISO_639_1::Korean},
		{"kj", LocaleISO_639_1::Kuanyama},
		{"ku", LocaleISO_639_1::Kurdish},
		{"lo", LocaleISO_639_1::Lao},
		{"la", LocaleISO_639_1::Latin},
		{"lv", LocaleISO_639_1::Latvian},
		{"li", LocaleISO_639_1::Limburgan},
		{"ln", LocaleISO_639_1::Lingala},
		{"lt", LocaleISO_639_1::Lithuanian},
		{"lu", LocaleISO_639_1::Luba_Katanga},
		{"lb", LocaleISO_639_1::Luxembourgish},
		{"mk", LocaleISO_639_1::Macedonian},
		{"mg", LocaleISO_639_1::Malagasy},
		{"ms", LocaleISO_639_1::Malay},
		{"ml", LocaleISO_639_1::Malayalam},
		{"mt", LocaleISO_639_1::Maltese},
		{"gv", LocaleISO_639_1::Manx},
		{"mi", LocaleISO_639_1::Maori},
		{"mr", LocaleISO_639_1::Marathi},
		{"mh", LocaleISO_639_1::Marshallese},
		{"mn", LocaleISO_639_1::Mongolian},
		{"na", LocaleISO_639_1::Nauru},
		{"nv", LocaleISO_639_1::Navajo},
		{"nd", LocaleISO_639_1::North_Ndebele},
		{"nr", LocaleISO_639_1::South_Ndebele},
		{"ng", LocaleISO_639_1::Ndonga},
		{"ne", LocaleISO_639_1::Nepali},
		{"no", LocaleISO_639_1::Norwegian},
		{"nb", LocaleISO_639_1::Norwegian_Bokmal},
		{"nn", LocaleISO_639_1::Norwegian_Nynorsk},
		{"ii", LocaleISO_639_1::Sichuan_Yi},
		{"oc", LocaleISO_639_1::Occitan},
		{"oj", LocaleISO_639_1::Ojibwa},
		{"or", LocaleISO_639_1::Oriya},
		{"om", LocaleISO_639_1::Oromo},
		{"os", LocaleISO_639_1::Ossetian},
		{"pi", LocaleISO_639_1::Pali},
		{"ps", LocaleISO_639_1::Pashto},
		{"fa", LocaleISO_639_1::Persian},
		{"pl", LocaleISO_639_1::Polish},
		{"pt", LocaleISO_639_1::Portuguese},
		{"pa", LocaleISO_639_1::Punjabi},
		{"qu", LocaleISO_639_1::Quechua},
		{"ro", LocaleISO_639_1::Romanian},
		{"rm", LocaleISO_639_1::Romansh},
		{"rn", LocaleISO_639_1::Rundi},
		{"ru", LocaleISO_639_1::Russian},
		{"se", LocaleISO_639_1::Northern_Sami},
		{"sm", LocaleISO_639_1::Samoan},
		{"sg", LocaleISO_639_1::Sango},
		{"sa", LocaleISO_639_1::Sanskrit},
		{"sc", LocaleISO_639_1::Sardinian},
		{"sr", LocaleISO_639_1::Serbian},
		{"sn", LocaleISO_639_1::Shona},
		{"sd", LocaleISO_639_1::Sindhi},
		{"si", LocaleISO_639_1::Sinhala},
		{"sk", LocaleISO_639_1::Slovak},
		{"sl", LocaleISO_639_1::Slovenian},
		{"so", LocaleISO_639_1::Somali},
		{"st", LocaleISO_639_1::Southern},
		{"es", LocaleISO_639_1::Spanish},
		{"su", LocaleISO_639_1::Sundanese},
		{"sw", LocaleISO_639_1::Swahili},
		{"ss", LocaleISO_639_1::Swati},
		{"sv", LocaleISO_639_1::Swedish},
		{"tl", LocaleISO_639_1::Tagalog},
		{"ty", LocaleISO_639_1::Tahitian},
		{"tg", LocaleISO_639_1::Tajik},
		{"ta", LocaleISO_639_1::Tamil},
		{"tt", LocaleISO_639_1::Tatar},
		{"te", LocaleISO_639_1::Telugu},
		{"th", LocaleISO_639_1::Thai},
		{"bo", LocaleISO_639_1::Tibetan},
		{"ti", LocaleISO_639_1::Tigrinya},
		{"to", LocaleISO_639_1::Tonga},
		{"ts", LocaleISO_639_1::Tsonga},
		{"tn", LocaleISO_639_1::Tswana},
		{"tr", LocaleISO_639_1::Turkish},
		{"tk", LocaleISO_639_1::Turkmen},
		{"tw", LocaleISO_639_1::Twi},
		{"ug", LocaleISO_639_1::Uighur},
		{"uk", LocaleISO_639_1::Ukrainian},
		{"ur", LocaleISO_639_1::Urdu},
		{"uz", LocaleISO_639_1::Uzbek},
		{"ve", LocaleISO_639_1::Venda},
		{"vi", LocaleISO_639_1::Vietnamese},
		{"vo", LocaleISO_639_1::Volapuk},
		{"wa", LocaleISO_639_1::Walloon},
		{"cy", LocaleISO_639_1::Welsh},
		{"wo", LocaleISO_639_1::Wolof},
		{"xh", LocaleISO_639_1::Xhosa},
		{"yi", LocaleISO_639_1::Yiddish},
		{"yo", LocaleISO_639_1::Yoruba},
		{"za", LocaleISO_639_1::Zhuang},
		{"zu", LocaleISO_639_1::Zulu},
	};
	const int s_data_count = ARRAY_LITERAL_SIZE(s_data);

	const std::map<std::string, LocaleISO_639_1>& GetStringEnumMap()
	{
		static std::map<std::string, LocaleISO_639_1> s_map;
		if (0 == s_map.size())
		{
			for (int index = 0; index < s_data_count; ++index)
			{
				const Data& data = s_data[index];
				s_map[data._data] = data._enum;
			}
		}
		return s_map;
	}

	const std::map<LocaleISO_639_1, std::string>& GetEnumStringMap()
	{
		static std::map<LocaleISO_639_1, std::string> s_map;
		if (0 == s_map.size())
		{
			for (int index = 0; index < s_data_count; ++index)
			{
				const Data& data = s_data[index];
				s_map[data._enum] = data._data;
			}
		}
		return s_map;
	}
}

template <> 
const LocaleISO_639_1 EnumSoftBind<LocaleISO_639_1>::EnumFromString(const std::string& in_locale)
{
	const auto& map = GetStringEnumMap();
	auto found = map.find(in_locale);
	if (found != map.end())
	{
		return found->second;
	}
	return LocaleISO_639_1::Default;
}

template <> 
const std::string EnumSoftBind<LocaleISO_639_1>::EnumToString(const LocaleISO_639_1 in_locale)
{
	const auto& map = GetEnumStringMap();
	auto found = map.find(in_locale);
	if (found != map.end())
	{
		return found->second;
	}
	return s_default_data;
}

