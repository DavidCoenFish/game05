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
		PathFinderTile(const PathFinderTile& in_source);
		~PathFinderTile();

		// return true if data changed
		const bool SetTraversable(const bool in_traversable, const int32_t in_offset);

		const uint8_t GetSubRegionId(const int32_t in_offset);
		/// visit the location the given region touches on other tiles
		void VisitRegionTouchingLocation(
			const std::function<void(const VectorShort2& in_location)>& in_callback, 
			const uint8_t in_tile_sub_region_id,	
			const VectorShort2& in_origin
			);

		/// visit all the location inside the region on our tile
		void VisitRegionLocation(
			const std::function<void(const VectorShort2& in_location)>& in_callback, 
			const uint8_t in_tile_sub_region_id,	
			const VectorShort2& in_origin
			);

		const int32_t GetTileChangeId() const { return _tile_change_id; }

	private:
		int32_t _tile_change_id = 0;
		// 16 x 16 = 256
		std::bitset<256> _traversable_map = {};

		// 16 x 16 = 256
		std::array<PathFinderTileDistance, 256> _distance_array = {};

		/// should this really live here? or needs to move into an array under the region manager? this is region data for the tile...
		std::unique_ptr<PathFinderTileRegion> _tile_region = {};

	};
};