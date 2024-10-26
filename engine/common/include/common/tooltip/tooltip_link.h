#pragma once

#include "common/tooltip/i_tooltip.h"
#include "common/locale/locale_enum.h"

class Tooltip;

class TooltipLink : public ITooltip
{
public:
	explicit TooltipLink(
		const std::string in_text = {},
		const LocaleISO_639_1 in_locale = LocaleISO_639_1::Invalid,
		const int32_t in_verbosity = 0,
		const std::vector<std::shared_ptr<ITooltip>>& in_children = {}
		);

private:
	virtual void Visit(const std::function<const bool(const std::string&, const LocaleISO_639_1, const int32_t, const int32_t)>& in_visitor, const int32_t in_depth = 0) const override;
	virtual const LocaleISO_639_1 GetLocale() const override { return _locale; }

private:
	std::string _text = {};
	LocaleISO_639_1 _locale = LocaleISO_639_1::Invalid;
	int32_t _verbosity = 0;
	std::vector<std::shared_ptr<ITooltip>> _children = {};

};
