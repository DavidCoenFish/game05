#pragma once

#include "common/locale/i_locale_string_format.h"

class LocaleSystem;

class LocaleStringFormatSimple : public ILocaleStringFormat
{
public:
	LocaleStringFormatSimple(const std::vector<std::string>& in_data);

	const std::string& GetResult() const { return _result; }

private:
	void AccumulateString(const std::string& in_value) override;
	void AccumulateToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) override;

private:
	std::vector<std::string> _data;
	std::string _result;
	
};
