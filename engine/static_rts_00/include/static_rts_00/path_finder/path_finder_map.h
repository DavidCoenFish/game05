#pragma once

class VectorShort2;

namespace static_rts_00
{
	class Map
	{
	public:
		typedef std::function<void(const VectorShort2& in_location, const uint8_t in_data)> TMapChangeCallback;

		Map(const int in_width,
			const int in_height,
			const std::vector<uint8_t>& in_data,
			const uint8_t in_invalid_cell_data
			);
		const uint8_t GetCell(const VectorShort2& in_location) const;
		void SetCell(const VectorShort2& in_location, const uint8_t in_data);

		void AddChangeCallback(const TMapChangeCallback& in_callback, const uint8_t in_dirty_mask);

		const int GetWidth() const { return _width; }
		const int GetHeight() const { return _height; }

	private:
		const int _width;
		const int _height;
		const uint8_t _invalid_cell_data;
		std::vector<uint8_t> _data;

		std::vector<std::pair<uint8_t, TMapChangeCallback>> _dirty_mask_change_callback_array;
	};
};