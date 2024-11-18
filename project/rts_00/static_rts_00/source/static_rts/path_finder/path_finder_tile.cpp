#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_tile.h"
#include "common/macro.h"

StaticRts::PathFinderTile::PathFinderTile()
{
	// nop
}

const bool StaticRts::PathFinderTile::SetTraversable(const bool in_traversable, const int32_t in_offset)
{
	bool result = false;
	DSC_ASSERT(0 <= in_offset && in_offset < 256, "invalid param");
	if (in_traversable != _traversable_map[in_offset])
	{
		_traversable_map[in_offset] = in_traversable;
		_tile_change_id += 1;
		result = true;
	}
	return result;
}

const uint8_t StaticRts::PathFinderTile::GetSubRegionId(const int32_t in_offset)
{
	DSC_ASSERT((0 <= in_offset) && (in_offset < 256), "");
	PathFinderTileDistance& distance_cache = _distance_array[in_offset];

	distance_cache.RefreashDistanceMap(_tile_change_id, _traversable_map, in_offset);

	// refreash region map?
	if (_region_change_id != _tile_change_id)
	{
		_region_change_id = _tile_change_id;
		_region_map.fill(0);
		uint8_t region_id_trace = 1; // or in_offset + 1? no, 255 + 1 would wrap to zero

		//for every non zero distance of distance_cache, set the _region_map

	}
}

