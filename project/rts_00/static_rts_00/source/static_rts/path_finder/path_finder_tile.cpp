#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_tile.h"
#include "common/macro.h"

StaticRts::PathFinderTile::PathFinderTile()
{
	// nop
}

void StaticRts::PathFinderTile::SetTraversable(const bool in_traversable, const int32_t in_offset)
{
	DSC_ASSERT(0 <= in_offset && in_offset < 256, "invalid param");
	if (in_traversable != _traversable_map[in_offset])
	{
		_traversable_map[in_offset] = in_traversable;
		_change_id += 1;
	}
	return;
}

