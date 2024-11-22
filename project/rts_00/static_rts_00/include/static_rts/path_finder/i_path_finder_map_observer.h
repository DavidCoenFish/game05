#pragma once

class VectorShort2;

namespace StaticRts
{
	class IPathFinderMapObserver
	{
	public:
		virtual void SetInitialData(
			const int32_t in_width,
			const int32_t in_height,
			const std::vector<uint8_t>& in_data
			)
		{
			in_width; in_height; in_data;
		};

		virtual void OnChange(const VectorShort2& in_location, const uint8_t in_data) = 0;
		//virtual const uint8_t GetMask() const = 0;
	};
}