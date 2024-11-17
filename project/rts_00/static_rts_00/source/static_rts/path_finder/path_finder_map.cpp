#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_map.h"

#include "static_rts/path_finder/i_path_finder_map_objserver.h"
#include "common/math/vector_short2.h"
#include "common/macro.h"

StaticRts::PathFinderMap::PathFinderMap(
	const int in_width,
	const int in_height,
	const uint8_t in_invalid_cell_data,
	const std::vector<uint8_t>& in_data
	)
	: _width(in_width)
	, _height(in_height)
	, _invalid_cell_data(in_invalid_cell_data)
	, _data(in_data)
{
	DSC_ASSERT(0 < in_width, "invalid param");
	DSC_ASSERT(0 < in_height, "invalid param");
	_data.resize(in_width * in_height, in_invalid_cell_data);
}

const uint8_t StaticRts::PathFinderMap::GetCell(const VectorShort2& in_location) const
{
	uint8_t result = _invalid_cell_data;
	int32_t offset = 0;
	if (true == MakeOffset(offset, _width, _height, in_location))
	{
		result = _data[offset];
	}
	return result;
}

void StaticRts::PathFinderMap::SetCell(const VectorShort2& in_location, const uint8_t in_data)
{
	int32_t offset = 0;
	if (false == MakeOffset(offset, _width, _height, in_location))
	{
		return;
	}

	const uint8_t old_data = _data[offset];
	if (old_data == in_data)
	{
		return;
	}

	_data[offset] = in_data;

	const uint8_t changed_bits = old_data ^ in_data;
	for (const auto& item : _dirty_mask_observer_array)
	{
		if (0 != (changed_bits & item.first))
		{
			item.second->OnChange(in_location, in_data);
		}
	}
}

void StaticRts::PathFinderMap::AddObserver(const std::shared_ptr<IPathFinderMapObserver>& in_observer, const uint8_t in_dirty_mask)
{
	_dirty_mask_observer_array.push_back(std::pair<uint8_t, IPathFinderMapObserver*>(in_dirty_mask, in_observer.get()));
	_observer_array.push_back(in_observer);
}

void StaticRts::PathFinderMap::RemoveObserver(IPathFinderMapObserver* const in_observer)
{
	_dirty_mask_observer_array.erase(std::remove_if(_dirty_mask_observer_array.begin(), _dirty_mask_observer_array.end(),
                             [in_observer](std::pair<uint8_t, IPathFinderMapObserver*>& pair) { return pair.second == in_observer; }));
	_observer_array.erase(std::remove_if(_observer_array.begin(), _observer_array.end(),
                             [in_observer](std::shared_ptr<IPathFinderMapObserver>& item) { return item.get() == in_observer; }));
}

const bool StaticRts::PathFinderMap::MakeOffset(
	int32_t& out_offset,
	const int32_t in_width,
	const int32_t in_height,
	const VectorShort2& in_location
	)
{
	const short x = in_location.GetX();
	const short y = in_location.GetY();
	if ((x < 0) || 
		(in_width <= x) ||
		(y < 0) ||
		(in_height <= y))
	{
		return false;
	}

	out_offset = x + (in_width * y);

	return true;
}
