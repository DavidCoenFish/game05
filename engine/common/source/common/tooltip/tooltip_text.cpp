#include "common/common_pch.h"
#include "common/tooltip/tooltip_text.h"

TooltipText::TooltipText(
	const std::string in_text,
	const LocaleISO_639_1 in_locale,
	const int32_t in_verbosity
	)
	: _text(in_text)
	, _locale(in_locale)
	, _verbosity(in_verbosity)
{
	// nop
}

void TooltipText::Visit(const std::function<const bool(const std::string&, const LocaleISO_639_1, const int32_t, const int32_t)>& in_visitor, const int32_t in_depth) const
{
	in_visitor(_text, _locale, _verbosity, in_depth);
}
