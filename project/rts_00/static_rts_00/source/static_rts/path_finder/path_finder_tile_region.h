#pragma once

class VectorShort2;

namespace StaticRts
{
	class PathFinderTileRegion
	{
	public:
		PathFinderTileRegion();

		void RefreashRegionMap(
			const int32_t in_tile_change_id, 
			const std::bitset<256>& in_traversable_map
			);

		/// for each region, if we are just interested in which border cells could touch neighbouring tiles, we could express that as a bit mask of 32 values
		/// we can not use that bitmask as the value in the _region_map as there may be regions that touch no neighbour adjacent cells
		/*
		if we were just a 4x4
			0,0		0,1		0,2		0,3
			1,0		1,1		1,2		1,3
			2,0		2,1		2,2		3,3
			3,0		3,1		3,2		3,3
		we could use a bitmask for [yes/no] cell open to touching neigbhour tile at
			[0,0 0,1 0,2 0,3][0,0 1,0 2,0 3,0][3,0 3,1 3,2 3,3][0,3 1,3 2,3 3,3]
		*/
		//struct Data
		//{
		//	Data(); //{ _touching_adjacent_tile_locations.reserve(32); }
		//	/// max size 32?
		//	//std::vector<VectorShort2> _touching_adjacent_tile_locations = {};
		//	/// max 8, offset on the given border that the region touches the edge of the "tile"
		//	std::vector<uint8_t> _touching_x_neg = {};
		//	std::vector<uint8_t> _touching_y_neg = {};
		//	std::vector<uint8_t> _touching_x_pos = {};
		//	std::vector<uint8_t> _touching_y_pos = {};
		//};

		/// return 0 if not in region, assumses RefreashRegionMap has been call.
		/// could pass param of RefreashRegionMap and call it enternally to ensure that it is called?
		/// in offset is indexs of the 8x8 data array (only use uint8_t when packing data, else try to work with int?)
		const uint8_t GetRegionID(
			const int32_t in_offset
			) const;

		const uint64_t GetRegionMask(
			const uint8_t in_id
			) const;

		/// invoke the callback with the location that the given region touches, outside our tile
		void VisitRegionTouchingLocation(
			const std::function<void(const VectorShort2& in_location)>& in_callback, 
			const uint8_t in_tile_sub_region_id,
			const VectorShort2& in_origin
			) const;

		/// visit all the location inside the region on our tile
		void VisitRegionLocation(
			const std::function<void(const VectorShort2& in_location)>& in_callback, 
			const uint8_t in_tile_sub_region_id,	
			const VectorShort2& in_origin
			) const;

	private:
		const bool PaintRegion(
			const uint8_t in_region_trace, 
			const int32_t in_offset, 
			const std::bitset<256>& in_traversable_map
			);
	
		void FloodFill(
			const uint8_t in_region_trace, 
			const int32_t in_offset, 
			const std::bitset<256>& in_traversable_map
			);

		const uint64_t CalculateRegionMask(const uint8_t in_region_trace) const;

	private:
		/// the change id of the tile we generated our region map against
		int32_t _tile_change_id = 0;

		/// paint a region id (region index + 1) on each traversable cell of the tile
		/// 16 x 16 = 256
		std::array<uint8_t, 256> _region_map = {};

		/// max size 128? (max how many disticts regions can fit on a 16x16) keep a mask of the open tile border cells each region touches
		std::vector<uint64_t> _region_mask_array = {};

	};
};