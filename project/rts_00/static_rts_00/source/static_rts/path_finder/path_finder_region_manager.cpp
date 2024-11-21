#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_region_manager.h"

#include "common/macro.h"
#include "common/math/vector_short2.h"
#include "static_rts/path_finder/path_finder_tile.h"

namespace
{
	const int32_t MakeKeyTileIndexSubRegion(	
		const int32_t in_tile_index,
		const uint8_t in_tile_sub_region_id)
	{
		// pretty fair assumption that in_tile_index fits in 2^24 = 16777216 = 4096 x 4096, which gives us a 65,536 x 65,536 cell grid
		return (in_tile_sub_region_id) | (in_tile_index << 8);
	}
}

StaticRts::PathFinderRegionManager::PathFinderRegionManager()
{
	// nop
}

const int32_t StaticRts::PathFinderRegionManager::CalculateRegion(
	const int32_t in_tile_index,
	const uint8_t in_tile_sub_region_id,
	const int32_t in_tile_manager_change_id, 
	const int32_t in_tile_width, 
	const int32_t in_tile_height, 
	//const std::vector<StaticRts::PathFinderTile>& in_tile_array
	IPathFinderRegionDataSource& in_data_source
	)
{
	RefreashRegionManager(in_tile_manager_change_id);

	const int32_t key = MakeKeyTileIndexSubRegion(in_tile_index, in_tile_sub_region_id);
	auto found = _map_tile_sub_region_id_to_region_id.find(key);
	if (found != _map_tile_sub_region_id_to_region_id.end())
	{
		return found->second; // - 1;
	}
	const int32_t region_id = static_cast<int32_t>(_region_array.size());
	_region_array.resize(region_id + 1);
	FlowRegion(region_id, _region_array.back(), in_tile_index, in_tile_sub_region_id, in_tile_width, in_tile_height, in_data_source);

	return region_id;
}

const bool StaticRts::PathFinderRegionManager::RegionContainsSubRegion(
	const int32_t in_region_id, 
	const int32_t in_tile_index,
	const uint8_t in_tile_sub_region_id
	)
{
	const int32_t key = MakeKeyTileIndexSubRegion(in_tile_index, in_tile_sub_region_id);
	const auto& region_key_set = _region_array[in_region_id]._set_tile_sub_region_id;
	auto found = region_key_set.find(key);
	return found != region_key_set.end();
}

void StaticRts::PathFinderRegionManager::RefreashRegionManager(
	const int32_t in_tile_manager_change_id
	)
{
	if (_tile_manager_change_id == in_tile_manager_change_id)
	{
		return;
	}
	_tile_manager_change_id = in_tile_manager_change_id;
	_map_tile_sub_region_id_to_region_id.clear();
	_region_array.clear();
}

void StaticRts::PathFinderRegionManager::FlowRegion(
	const int32_t in_region_id, 
	Data& in_region, 
	const int32_t in_tile_index,
	const uint8_t in_tile_sub_region_id,
	const int32_t in_tile_width, 
	const int32_t in_tile_height, 
	//const std::vector<StaticRts::PathFinderTile>& in_tile_array
	IPathFinderRegionDataSource& in_data_source
	)
{
	const int32_t key = MakeKeyTileIndexSubRegion(in_tile_index, in_tile_sub_region_id);
	if (in_region._set_tile_sub_region_id.end() != in_region._set_tile_sub_region_id.find(key))
	{
		return;
	}

	in_region._set_tile_sub_region_id.insert(key);
	_map_tile_sub_region_id_to_region_id[key] = in_region_id;// + 1;

	in_data_source.VisitRegionTouchingLocation([&](const int32_t in_tile_index_inner, const uint8_t in_tile_sub_region_id_inner)
		{
			this->FlowRegion(
				in_region_id, 
				in_region, 
				in_tile_index_inner,
				in_tile_sub_region_id_inner,
				in_tile_width, 
				in_tile_height, 
				in_data_source
				);
		}, 
		in_tile_index, 
		in_tile_sub_region_id,
		VectorShort2(
			static_cast<int16_t>((in_tile_index % in_tile_width) * 16), 
			static_cast<int16_t>((in_tile_index / in_tile_width) * 16)
			)
		);
}

