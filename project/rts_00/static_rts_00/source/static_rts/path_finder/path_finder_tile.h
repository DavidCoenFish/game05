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
		void SetTraversable(const bool in_traversable, const int32_t in_offset);

		const int32_t GetChangeId() const { return _change_id; }

	private:
		int32_t _change_id = 0;
		std::bitset<256> _traversable_map = {};

		std::array<PathFinderTileDistance, 256> _distance_array = {};

	};
};