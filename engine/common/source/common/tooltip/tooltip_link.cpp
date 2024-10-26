#include "common/common_pch.h"
#include "common/tooltip/tooltip_link.h"


TooltipLink::TooltipLink(
	const std::string in_text,
	const LocaleISO_639_1 in_locale,
	const int32_t in_verbosity,
	const std::vector<std::shared_ptr<ITooltip>>& in_children
	)
	: _text(in_text)
	, _locale(in_locale)
	, _verbosity(in_verbosity)
	, _children(in_children)
{
	// nop
}

void TooltipLink::Visit(const std::function<const bool(const std::string&, const LocaleISO_639_1, const int32_t, const int32_t)>& in_visitor, const int32_t in_depth) const
{
	if (true == in_visitor(_text, _locale, _verbosity, in_depth))
	{
		for (const auto& item : _children)
		{
			item->Visit(in_visitor, in_depth + 1);
		}
	}
}
