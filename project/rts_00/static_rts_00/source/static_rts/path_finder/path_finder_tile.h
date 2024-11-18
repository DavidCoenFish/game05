#pragma once

#include "static_rts/path_finder/path_finder_tile_distance.h"

class VectorShort2;


namespace StaticRts
{
	class PathFinderTileSubRegion
	{
	private:
		// [x+,y+, x-,y-].[max 8 locations in tile that are adjacent to neighbours and are taversable as part of this subregion]
	};

	class PathFinderTile
	{
	public:
		PathFinderTile();
		// return true if data changed
		const bool SetTraversable(const bool in_traversable, const int32_t in_offset);
		const uint8_t GetSubRegionId(const int32_t in_offset);

		const int32_t GetTileChangeId() const { return _tile_change_id; }

	private:
		int32_t _tile_change_id = 0;
		std::bitset<256> _traversable_map = {};

		std::array<PathFinderTileDistance, 256> _distance_array = {};

		int32_t _region_change_id = 0;
		//std::vector<int32_t> _distance_index_defining_region_array = {};
		/// arbitatially give an index to each region, but paint every cell of the region with a region id
		std::array<uint8_t, 256> _region_map = {};
		// more data for a region, precompute for each region what the offsets are for the tile boundry cells for the region?

	};
};