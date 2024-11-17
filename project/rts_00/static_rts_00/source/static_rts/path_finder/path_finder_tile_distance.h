#pragma once

class VectorShort2;

namespace StaticRts
{
	class PathFinderTileDistance
	{
	public:
		const bool IsPointAccessable(const VectorShort2& in_location);
	private:
		std::array<uint8_t, 256> _distance = {};
		uint8_t _sub_region_id = 0;
	};
};