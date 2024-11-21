#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_tile_distance.h"

#include "common/macro.h"

void StaticRts::PathFinderTileDistance::RefreashDistanceMap(
	const int32_t in_tile_change_id, 
	const std::bitset<256>& in_traversable_map,
	const int32_t in_offset
	)
{
	if (_tile_change_id == in_tile_change_id)
	{
		return;
	}

	_tile_change_id = in_tile_change_id;

	_distance.fill(0);

	FloodFillRegion(1, in_traversable_map, in_offset);
}

void StaticRts::PathFinderTileDistance::FloodFillRegion(const int32_t in_distance, 
	const std::bitset<256>& in_traversable_map,
	const int32_t in_offset)
{
	// bail if not a traversable cell
	if (false == in_traversable_map[in_offset])
	{
		return;
	}

	const uint8_t old_value = _distance[in_offset];
	if ((0 != old_value) && (old_value < in_distance))
	{
		return;
	}

	DSC_ASSERT((0 <= in_distance) && (in_distance < 256), "fail sanity check distance");
	_distance[in_offset] = static_cast<uint8_t>(in_distance);

	int32_t x = in_offset % 16;
	int32_t y = in_offset / 16;

	const int32_t new_distance = in_distance + 1;
	if (0 <= x - 1)
	{
		FloodFillRegion(new_distance, in_traversable_map, x - 1 + (y * 16));
	}
	if (x + 1 < 16)
	{
		FloodFillRegion(new_distance, in_traversable_map, x + 1 + (y * 16));
	}
	if (0 <= y - 1)
	{
		FloodFillRegion(new_distance, in_traversable_map, x + ((y - 1) * 16));
	}
	if (y + 1 < 16)
	{
		FloodFillRegion(new_distance, in_traversable_map, x + ((y + 1) * 16));
	}
}


