#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_tile_manager.h"

#include "common/macro.h"
#include "common/math/dsc_math.h"
#include "common/math/vector_short2.h"
#include "static_rts/path_finder/path_finder_tile.h"

namespace 
{
	/// what is the index of the tile in the tile array
	const int32_t CalculateTileIndex(const int32_t in_x, const int32_t in_y, const int32_t in_tile_width)
	{
		return ((in_y / 8) * in_tile_width) + (in_x / 8);
	}

	/// what is our offset within a tile
	const int32_t CalculateTileOffset(const int32_t in_x, const int32_t in_y)
	{
		return ((in_y % 8) * 8) + (in_x % 8);
	}
}

class PathFinderTileManagerImplementation
{
public:
	PathFinderTileManagerImplementation(const StaticRts::PathFinderTileManager::TIsMapDataTraversable in_is_data_traversable_function)
		: _is_data_traversable_function(in_is_data_traversable_function)
	{
		DSC_ASSERT(nullptr != _is_data_traversable_function, "invalid param");
	}

	void SetInitialData(
		const int32_t in_width,
		const int32_t in_height,
		const std::vector<uint8_t>& in_data
		)
	{
		_tile_width = DscMath::IntCeilingDiv(in_width, 8);
		_tile_height = DscMath::IntCeilingDiv(in_height, 8);

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

	void OnChange(const VectorShort2& in_location, const uint8_t in_data)
	{
		const int32_t x = in_location.GetX();
		const int32_t y = in_location.GetY();
		const int32_t tile_index = CalculateTileIndex(x, y, _tile_width);
		const int32_t tile_offset = CalculateTileOffset(x, y);

		const bool traversable = (*_is_data_traversable_function)(in_data);
		_tile_array[tile_index].SetTraversable(traversable, tile_offset);
	}

private:
	StaticRts::PathFinderTileManager::TIsMapDataTraversable _is_data_traversable_function = nullptr;

	int32_t _tile_width = 0;
	int32_t _tile_height = 0;
	std::vector<StaticRts::PathFinderTile> _tile_array = {};

};

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

void StaticRts::PathFinderTileManager::OnChange(const VectorShort2& in_location, const uint8_t in_data)
{
	_implementation->OnChange(in_location, in_data);
}


