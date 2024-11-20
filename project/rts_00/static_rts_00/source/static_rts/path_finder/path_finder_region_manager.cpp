#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_tile_region.h"

#include "common/macro.h"

StaticRts::PathFinderTileRegion::PathFinderTileRegion()
{
	_region_mask_array.reserve(128);
}

void StaticRts::PathFinderTileRegion::RefreashRegionMap(
	const int32_t in_tile_change_id, 
	const std::bitset<256>& in_traversable_map
	)
{
	if (_tile_change_id == in_tile_change_id)
	{
		return;
	}

	_tile_change_id = in_tile_change_id;

	_region_map.fill(0);
	_region_mask_array.resize(0);

	// if every other cell was traversable (checkerboard) our region count would be 128?
	uint8_t region_trace = 0;
	for (int32_t index = 0; index < 256; ++index)
	{
		if (true == PaintRegion(region_trace, index, in_traversable_map))
		{
			region_trace += 1;
		}
	}
}

const uint8_t StaticRts::PathFinderTileRegion::GetRegionID(
	const int32_t in_offset
	)
{
	DSC_ASSERT((0 <= in_offset) && (in_offset < 256), "invalid param");
	return _region_map[in_offset];
}

const uint32_t StaticRts::PathFinderTileRegion::GetRegionMask(
	const uint8_t in_id
	)
{
	if (0 == in_id)
	{
		return 0;
	}
	return _region_mask_array[in_id - 1];
}


const bool StaticRts::PathFinderTileRegion::PaintRegion(
	const uint8_t in_region_trace, 
	const int32_t in_offset, 
	const std::bitset<256>& in_traversable_map
	)
{
	if ((false == in_traversable_map[in_offset]) || (0 != _region_map[in_offset]))
	{
		return false;
	}
	//_region_map[in_offset] = in_region_trace + 1;

	FloodFill(
		in_region_trace, 
		in_offset, 
		in_traversable_map
		);

	// in_region_trace + 1 as we used 1 based index as id
	const uint32_t region_mask = CalculateRegionMask(in_region_trace + 1);
	// in_region_trace + 1 make sure there is enough space in _region_mask_array for the region
	_region_mask_array.resize(in_region_trace + 1);
	_region_mask_array[in_region_trace] = region_mask;
	return true;
}

void StaticRts::PathFinderTileRegion::FloodFill(
	const uint8_t in_region_trace, 
	const int32_t in_offset, 
	const std::bitset<256>& in_traversable_map
	)
{
	if ((false == in_traversable_map[in_offset]) || (0 != _region_map[in_offset]))
	{
		return;
	}

	// WARNING, 1 based index to _region_mask_array, don't want to use 0 baseded as the 0 index clashes with cleared to zero data
	_region_map[in_offset] = in_region_trace + 1;

	int32_t x = in_offset % 8;
	int32_t y = in_offset / 8;
	if (0 <= x - 1)
	{
		FloodFill(in_region_trace, x - 1 + (y * 8), in_traversable_map);
	}
	if (0 <= y - 1)
	{
		FloodFill(in_region_trace, x + ((y - 1) * 8), in_traversable_map);
	}
	if (x + 1 < 8)
	{
		FloodFill(in_region_trace, x + 1 + (y * 8), in_traversable_map);
	}
	if (y + 1 < 8)
	{
		FloodFill(in_region_trace, x + ((y + 1) * 8), in_traversable_map);
	}

	return;
}

/*
if we were just a 4x4
	0,0		0,1		0,2		0,3
	1,0		1,1		1,2		1,3
	2,0		2,1		2,2		3,3
	3,0		3,1		3,2		3,3
we could use a bitmask for [yes/no] cell open to touching neigbhour tile at
	[0,0 0,1 0,2 0,3][0,0 1,0 2,0 3,0][3,0 3,1 3,2 3,3][0,3 1,3 2,3 3,3]
*/
const uint32_t StaticRts::PathFinderTileRegion::CalculateRegionMask(const uint8_t in_region_id) const
{
	uint32_t mask = 0;
	int32_t bit_shift_trace = 0;
	//[0,0 0,1 0,2 0,3]
	for (int32_t index = 0; index < 8; ++index)
	{
		const int32_t offset = index;
		if (in_region_id == _region_map[offset])
		{
			mask |= (1 << bit_shift_trace);
		}
		bit_shift_trace += 1;
	}
	//[0,0 1,0 2,0 3,0]
	for (int32_t index = 0; index < 8; ++index)
	{
		const int32_t offset = index * 8;
		if (in_region_id == _region_map[offset])
		{
			mask |= (1 << bit_shift_trace);
		}
		bit_shift_trace += 1;
	}
	//[3,0 3,1 3,2 3,3]
	for (int32_t index = 0; index < 8; ++index)
	{
		const int32_t offset = index + (7 * 8);
		if (in_region_id == _region_map[offset])
		{
			mask |= (1 << bit_shift_trace);
		}
		bit_shift_trace += 1;
	}
	//[0,3 1,3 2,3 3,3]
	for (int32_t index = 0; index < 8; ++index)
	{
		const int32_t offset = (index * 8) + 7;
		if (in_region_id == _region_map[offset])
		{
			mask |= (1 << bit_shift_trace);
		}
		bit_shift_trace += 1;
	}
	return mask;
}
