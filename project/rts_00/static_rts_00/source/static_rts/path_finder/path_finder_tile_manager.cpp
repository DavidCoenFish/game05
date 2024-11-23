#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_tile_manager.h"

#include "common/macro.h"
#include "common/math/dsc_math.h"
#include "common/math/vector_short2.h"
#include "static_rts/path_finder/i_path_finder_path.h"
#include "static_rts/path_finder/path_finder_tile.h"
#include "static_rts/path_finder/path_finder_region_manager.h"
#include "static_rts/path_finder/path_finder_path_data_coincident.h"
#include "static_rts/path_finder/path_finder_path_data.h"

class PathFinderTileManagerImplementation
{
public:
	PathFinderTileManagerImplementation(const StaticRts::PathFinderTileManager::TIsMapDataTraversable in_is_data_traversable_function);
	~PathFinderTileManagerImplementation();

	void SetInitialData(
		const int32_t in_width,
		const int32_t in_height,
		const std::vector<uint8_t>& in_data
		);
	void OnChange(const VectorShort2& in_location, const uint8_t in_data);
	const bool IsSameRegion(const VectorShort2& in_a, const VectorShort2& in_b);
	const bool GetTileSubRegionId(int32_t& out_tile_index, uint8_t& out_tile_sub_region_id, const VectorShort2& in_location);
	void VisitRegionTouchingLocation(
		const std::function<void(const int32_t, const uint8_t)>& in_callback,
		const int32_t in_tile_index, 
		const uint8_t in_tile_sub_region_id,
		const VectorShort2& in_origin
		);
	void InitialisePath(
		StaticRts::IPathFinderPath& in_path, 
		const VectorShort2& in_current_location, 
		const VectorShort2& in_target_location
		);
	const bool RefreashPath(StaticRts::IPathFinderPath& in_path);

private:
	StaticRts::PathFinderTileManager::TIsMapDataTraversable _is_data_traversable_function = nullptr;

	int32_t _tile_width = 0;
	int32_t _tile_height = 0;
	std::vector<StaticRts::PathFinderTile> _tile_array = {};
	int32_t _tile_manager_change_id = 0;

	std::unique_ptr<StaticRts::PathFinderRegionManager> _region_manager;
};


namespace 
{
	/// what is the index of the tile in the tile array
	const int32_t CalculateTileIndex(const int32_t in_x, const int32_t in_y, const int32_t in_tile_width)
	{
		return ((in_y / 16) * in_tile_width) + (in_x / 16);
	}

	/// what is our offset within a tile
	const int32_t CalculateTileOffset(const int32_t in_x, const int32_t in_y)
	{
		return ((in_y % 16) * 16) + (in_x % 16);
	}

	class PathFinderRegionDataSource : public StaticRts::IPathFinderRegionDataSource
	{
	public:
		PathFinderRegionDataSource(PathFinderTileManagerImplementation& in_implementation) : _implementation(in_implementation) {}
	private:
		virtual void VisitRegionTouchingLocation(
			const std::function<void(const int32_t, const uint8_t)>& in_callback,
			const int32_t in_tile_index, 
			const uint8_t in_tile_sub_region_id,
			const VectorShort2& in_origin
			) override
		{
			_implementation.VisitRegionTouchingLocation(
				in_callback,
				in_tile_index, 
				in_tile_sub_region_id,
				in_origin
				);
		}
	private:
		PathFinderTileManagerImplementation& _implementation;
	};
}

PathFinderTileManagerImplementation::PathFinderTileManagerImplementation(const StaticRts::PathFinderTileManager::TIsMapDataTraversable in_is_data_traversable_function)
	: _is_data_traversable_function(in_is_data_traversable_function)
{
	DSC_ASSERT(nullptr != _is_data_traversable_function, "invalid param");
	_region_manager = std::make_unique<StaticRts::PathFinderRegionManager>();
}

PathFinderTileManagerImplementation::~PathFinderTileManagerImplementation()
{
	// nop
}

void PathFinderTileManagerImplementation::SetInitialData(
	const int32_t in_width,
	const int32_t in_height,
	const std::vector<uint8_t>& in_data
	)
{
	_tile_width = DscMath::IntCeilingDiv(in_width, 16);
	_tile_height = DscMath::IntCeilingDiv(in_height, 16);

	_tile_array.resize(_tile_width * _tile_height);
	int32_t trace = 0;
	for (int32_t index_y = 0; index_y < in_height; ++index_y)
	{
		for (int32_t index_x = 0; index_x < in_width; ++index_x)
		{
			const int32_t tile_index = CalculateTileIndex(index_x, index_y, _tile_width);
			const uint8_t data = in_data[trace];
			trace += 1;

			const bool traversable = (*_is_data_traversable_function)(data);
			const int32_t tile_offset = CalculateTileOffset(index_x, index_y);
			_tile_array[tile_index].SetTraversable(traversable, tile_offset);
		}
	}
}

void PathFinderTileManagerImplementation::OnChange(const VectorShort2& in_location, const uint8_t in_data)
{
	const int32_t x = in_location.GetX();
	const int32_t y = in_location.GetY();
	const int32_t tile_index = CalculateTileIndex(x, y, _tile_width);
	const int32_t tile_offset = CalculateTileOffset(x, y);

	const bool traversable = (*_is_data_traversable_function)(in_data);
	if (true == _tile_array[tile_index].SetTraversable(traversable, tile_offset))
	{
		_tile_manager_change_id += 1;
	}
}

const bool PathFinderTileManagerImplementation::IsSameRegion(const VectorShort2& in_a, const VectorShort2& in_b)
{
	int32_t tile_index_a = 0;
	uint8_t tile_sub_region_id_a = 0;
	if (false == GetTileSubRegionId(tile_index_a, tile_sub_region_id_a, in_a))
	{
		return false;
	}
	int32_t tile_index_b = 0;
	uint8_t tile_sub_region_id_b = 0;
	if (false == GetTileSubRegionId(tile_index_b, tile_sub_region_id_b, in_b))
	{
		return false;
	}

	// easy case, both locations on the same tile and subregion
	if ((tile_index_a == tile_index_b) && (tile_sub_region_id_a == tile_sub_region_id_b))
	{
		return true;
	}

	PathFinderRegionDataSource data_source(*this);

	// todo: do the regions end up touching
	//const int32_t region_id = _region_manager->CalculateRegion(tile_index_a, tile_sub_region_id_a, _tile_manager_change_id, _tile_width, _tile_height, _tile_array);
	const int32_t region_id = _region_manager->CalculateRegion(tile_index_a, tile_sub_region_id_a, _tile_manager_change_id, _tile_width, _tile_height, data_source);
	if (true == _region_manager->RegionContainsSubRegion(region_id, tile_index_b, tile_sub_region_id_b))
	{
		return true;
	}

	return false;
}

const bool PathFinderTileManagerImplementation::GetTileSubRegionId(int32_t& out_tile_index, uint8_t& out_tile_sub_region_id, const VectorShort2& in_location)
{
	const int32_t x = in_location.GetX();
	const int32_t y = in_location.GetY();
	const int32_t tile_index = CalculateTileIndex(x, y, _tile_width);
	const int32_t tile_offset = CalculateTileOffset(x, y);

	const int8_t sub_region = _tile_array[tile_index].GetSubRegionId(tile_offset);
	if (0 == sub_region)
	{
		return false;
	}

	out_tile_index = tile_index;
	out_tile_sub_region_id = sub_region;

	return true;
}

void PathFinderTileManagerImplementation::VisitRegionTouchingLocation(
	const std::function<void(const int32_t, const uint8_t)>& in_callback,
	const int32_t in_tile_index, 
	const uint8_t in_tile_sub_region_id,
	const VectorShort2& in_origin
	)
{
	_tile_array[in_tile_index].VisitRegionTouchingLocation(
		[&](const VectorShort2& in_offset){
			int32_t tile_index = 0;
			uint8_t tile_sub_region_id = 0;
			const int16_t x = in_offset.GetX();
			const int16_t y = in_offset.GetY();
			if ((x < 0) || 
				((_tile_width * 16) <= x) ||
				(y < 0) ||
				((_tile_height * 16) <= y))
			{
				return;
			}

			if (true == this->GetTileSubRegionId(tile_index, tile_sub_region_id, in_offset))
			{
				in_callback(tile_index, tile_sub_region_id);
			}
		},
		in_tile_sub_region_id,
		in_origin
		);
}

void PathFinderTileManagerImplementation::InitialisePath(
	StaticRts::IPathFinderPath& in_path, 
	const VectorShort2& in_current_location, 
	const VectorShort2& in_target_location
	)
{
	if (in_current_location == in_target_location)
	{
		// presume the caller wants something, though me may not do much when the start and end are the same cell/location
		in_path.SetPathData(std::make_shared<StaticRts::PathFinderPathDataCoincident>(in_current_location));
		return;
	}

	const bool same_region = IsSameRegion(in_current_location, in_target_location);
	if (true == same_region)
	{
		// find the shortest path to the target
		int32_t tile_index = 0;
		uint8_t tile_sub_region_id = 0;
		if (false == GetTileSubRegionId(tile_index, tile_sub_region_id, in_current_location))
		{
			in_path.SetPathData(nullptr);
			return;
		}

		PathFinderRegionDataSource data_source(*this);
		const int32_t region_id = _region_manager->CalculateRegion(tile_index, tile_sub_region_id, _tile_manager_change_id, _tile_width, _tile_height, data_source);



	}
	else
	{
		// find the dest point nearest to the target
	}
}

const bool PathFinderTileManagerImplementation::RefreashPath(StaticRts::IPathFinderPath& in_path)
{
}

StaticRts::PathFinderTileManager::PathFinderTileManager(
	const TIsMapDataTraversable in_is_data_traversable_function
	)
{
	_implementation = std::make_unique<PathFinderTileManagerImplementation>(in_is_data_traversable_function);
}

StaticRts::PathFinderTileManager::~PathFinderTileManager()
{
	// nop
}

void StaticRts::PathFinderTileManager::SetInitialData(
	const int32_t in_width,
	const int32_t in_height,
	const std::vector<uint8_t>& in_data
	)
{
	_implementation->SetInitialData(in_width, in_height, in_data);
}

void StaticRts::PathFinderTileManager::InitialisePath(
	IPathFinderPath& in_path, 
	const VectorShort2& in_current_location, 
	const VectorShort2& in_target_location
	)
{
	_implementation->InitialisePath(in_path, in_current_location, in_target_location);
	return;
}

/// update (also allow implimentation to advance location on) the path, return false if path traversal finished
const bool StaticRts::PathFinderTileManager::RefreashPath(IPathFinderPath& in_path)
{
	return _implementation->RefreashPath(in_path);
}

void StaticRts::PathFinderTileManager::OnChange(const VectorShort2& in_location, const uint8_t in_data)
{
	_implementation->OnChange(in_location, in_data);
}

const bool StaticRts::PathFinderTileManager::IsSameRegion(const VectorShort2& in_a, const VectorShort2& in_b)
{
	return _implementation->IsSameRegion(in_a, in_b);
}

