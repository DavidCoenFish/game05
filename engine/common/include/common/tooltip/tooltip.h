#pragma once

//#include "common/tooltip/tooltip_data.h"

struct TooltipData;

/*
i_tooltip_data::PerformLinkAction? PopupNewTooltip, Follow Link...
for now, just have tooltips hold an array of pairs/data of (text, optional tooltip)

*/
class Tooltip
{
public:
	Tooltip(const std::vector< TooltipData >& in_data);
	~Tooltip();

	const std::vector< TooltipData >& GetData() const { return _data; }
private:
	const std::vector< TooltipData > _data;

};
