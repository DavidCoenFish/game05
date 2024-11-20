#pragma once

class VectorShort2;

namespace StaticRts
{
	class PathFinderRegionManager
	{
	public:
		PathFinderRegionManager();

		const int32_t CalculateRegion(
			const int32_t in_tile_index,
			const uint8_t in_tile_sub_region_id,
			const int32_t in_tile_manager_change_id, 
			const int32_t in_tile_width, 
			const int32_t in_tile_height, 
			const std::vector<StaticRts::PathFinderTile>& in_tile_array
			);
		const bool RegionContainsSubRegion(
			const int32_t in_region_id, 
			const int32_t in_tile_index,
			const uint8_t in_tile_sub_region_id
			);


	private:
		/// clear cache if in_tile_manager_change_id != _tile_manager_change_id
		void RefreashRegionManager(
			const int32_t in_tile_manager_change_id
			);

	private:
		/// the change id of the tile manager we generated our region map against
		int32_t _tile_manager_change_id = 0;

		/// what is the id (index + 1) of the region that the 
		std::map<int32_t, int32_t> _map_tile_sub_region_id_to_region_id; 

		/// what are the tiles/sub regions that a region contains
		struct Data
		{
			std::set<int32_t> _set_tile_sub_region_id; 
		};
		std::vector<std::shared_ptr<Data>> _region_array = {};

	};
};