#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_tile.h"

#include "common/macro.h"
#include "static_rts/path_finder/path_finder_tile_region.h"

StaticRts::PathFinderTile::PathFinderTile()
{
	_tile_region = std::make_unique<PathFinderTileRegion>();
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
	//PathFinderTileDistance& distance_cache = _distance_array[in_offset];
	//distance_cache.RefreashDistanceMap(_tile_change_id, _traversable_map, in_offset);

	_tile_region->RefreashRegionMap(_tile_change_id, _traversable_map);
	const uint8_t id = _tile_region->GetRegionID(in_offset);
	return id;
}

