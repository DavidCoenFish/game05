#pragma once

class Tooltip;
enum class LocaleISO_639_1;

class ITooltip
{
public:
	virtual ~ITooltip(){};

	/// visitor function should return true if it want to be invoked for the children of the current node
	virtual void Visit(const std::function<const bool(const std::string&, const LocaleISO_639_1, const int32_t, const int32_t)>& in_visitor, const int32_t in_depth = 0) const = 0;

	virtual const LocaleISO_639_1 GetLocale() const = 0;
};
