#pragma once

class VectorShort2;

namespace StaticRts
{
	class PathFinderTileDistance
	{
	public:
		void RefreashDistanceMap(
			const int32_t in_tile_change_id, 
			const std::bitset<256>& in_traversable_map,
			const int32_t in_offset
			);

		//const bool IsPointAccessable(const VectorShort2& in_location);
		//const bool IsPointAccessable(const int32_t in_offset);
		//const uint8_t GetSubRegionId(const VectorShort2& in_location);

	private:
		void FloodFillRegion(const int32_t in_depth, 
				const std::bitset<256>& in_traversable_map,
				const int32_t in_offset);

	private:
		/// the change id of the tile we generated our distance map against
		int32_t _tile_change_id = 0;
		std::array<uint8_t, 256> _distance = {};
		uint8_t _sub_region_id = 0;

	};
};