#pragma once

#include "common/locale/i_locale_string_format.h"

class LocaleSystem;

class LocaleStringFormatLambda : public ILocaleStringFormat
{
public:
	LocaleStringFormatLambda(const std::function<const std::string(const LocaleISO_639_1, const std::string&, const int)>& in_callback);

	const std::string& GetResult() const { return _result; }

private:
	void AccumulateString(const std::string& in_value, const LocaleISO_639_1 in_locale) override;
	void AccumulateToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) override;

private:
	std::function<const std::string(const LocaleISO_639_1, const std::string&, const int)> _callback;
	std::string _result;

};
