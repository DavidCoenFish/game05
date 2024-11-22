#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_tile.h"

#include "common/macro.h"
#include "static_rts/path_finder/path_finder_tile_region.h"

StaticRts::PathFinderTile::PathFinderTile()
{
	_tile_region = std::make_unique<StaticRts::PathFinderTileRegion>();
}

StaticRts::PathFinderTile::~PathFinderTile()
{
	// Nop
}

StaticRts::PathFinderTile::PathFinderTile(const PathFinderTile& in_source)
{
	_tile_region = std::make_unique<StaticRts::PathFinderTileRegion>();
	_tile_change_id = in_source._tile_change_id;
	_traversable_map = in_source._traversable_map;
	_distance_array = in_source._distance_array;
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

void StaticRts::PathFinderTile::VisitRegionTouchingLocation(
	const std::function<void(const VectorShort2& in_location)>& in_callback, 
	const uint8_t in_tile_sub_region_id,
	const VectorShort2& in_origin
	) 
{
	_tile_region->VisitRegionTouchingLocation(in_callback, in_tile_sub_region_id, in_origin);
}

void StaticRts::PathFinderTile::VisitRegionLocation(
	const std::function<void(const VectorShort2& in_location)>& in_callback, 
	const uint8_t in_tile_sub_region_id,	
	const VectorShort2& in_origin
	)
{
	_tile_region->VisitRegionLocation(in_callback, in_tile_sub_region_id, in_origin);
}

