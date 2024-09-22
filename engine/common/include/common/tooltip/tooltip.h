#pragma once

#include "common/tooltip/tooltip_data.h"

class Tooltip
{
public:
	Tooltip(const std::vector< TooltipData >& in_data);
	~Tooltip();

	const std::vector< TooltipData >& GetData() const { return _data; }
private:
	const std::vector< TooltipData > _data;

};
