#pragma once

class Tooltip;

/// does this need to be a hyperlink style "preform action" interface?
/// something that clicking some text can result in, like opening another tooltip
/// or optioning a window? or just start with nested tooltips
class ITooltipAction
{
public:
	virtual ~ITooltipAction(){};
    //virtual void PerformAction() = 0; 
	virtual std::shared_ptr<Tooltip> GenerateTooltip() = 0;

};
