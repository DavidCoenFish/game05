#pragma once

class VectorShort2;

namespace StaticRts
{
	/// didn't want the child to know the details of the parent's implimentation, so use an interface
	class IPathFinderRegionDataSource 
	{
	public:
		virtual void VisitRegionTouchingLocation(
			const std::function<void(const int32_t, const uint8_t)>& in_callback,
			const int32_t in_tile_index, 
			const uint8_t in_tile_sub_region_id,
			const VectorShort2& in_origin
			) = 0;
	};


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
			//const std::vector<StaticRts::PathFinderTile>& in_tile_array
			//const std::function<const bool(int32_t&, uint8_t&, const VectorShort2&)>& in_data_source
			IPathFinderRegionDataSource& in_data_source
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

		struct Data;
		void FlowRegion(
			const int32_t in_region_id, 
			Data& in_region, 
			const int32_t in_tile_index,
			const uint8_t in_tile_sub_region_id,
			const int32_t in_tile_width, 
			const int32_t in_tile_height, 
			IPathFinderRegionDataSource& in_data_source
			);

	private:
		/// the change id of the tile manager we generated our region map against
		int32_t _tile_manager_change_id = 0;

		/// what is the id of the region that the <tile index, tile sub region id> map to
		std::map<int32_t, int32_t> _map_tile_sub_region_id_to_region_id = {}; 

		/// what are the <tile index, tile sub region id> regions that a region contains
		struct Data
		{
			std::set<int32_t> _set_tile_sub_region_id = {}; 
		};
		/// could be shared_ptr<Data>, 
		std::vector<Data> _region_array = {};

	};
};