#pragma once

class ITooltipAction;

struct TooltipData
{
public:
    std::string _text;
    /// if this is not null, then the text is a link which can open a new tooltip
    std::shared_ptr<ITooltipAction> _source;

};
