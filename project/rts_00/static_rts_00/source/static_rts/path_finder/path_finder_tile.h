#pragma once

#include "static_rts/path_finder/path_finder_tile_distance.h"

class VectorShort2;


namespace StaticRts
{
	class PathFinderTileRegion;

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

		std::unique_ptr<PathFinderTileRegion> _tile_region = nullptr;

	};
};