#pragma once

class VectorShort2;

namespace StaticRts
{
	class IPathFinderMapObserver;

	/// for example, data
	/// bit 0 - true, traversable by walking units
	/// bit 1 - true, traversable by burrowed units
	/// bit 2 - true, traversable by flying units
	/// bit 3 - true, blocked by resource / gatherable 
	/// bit 4 - true, blocked by building / destructable terrain
	/// bit 5 - true, blocked by map [map script disabled area?]
	/// would it be fun to allow the out of bounds data (invalid_cell_data) to allow flight?
	class PathFinderMap
	{
	public:
		/// alternate to TMapChangeCallback would be a IMapObserver::OnMapChange(location, data), ::GetMask()
		/// advantage, could document lifespan relationship better as interface object than lambda (when does target of lambda get dtor)
		/// disadvantage, tempation to keep on checking if object alive, ie, cost derefereing weak pointer, locking shared ptr
		/// could just pass in raw poointer to interface and have a AddObserver. RemoveObserver?
		//typedef std::function<void(const VectorShort2& in_location, const uint8_t in_data)> TMapChangeCallback;

		PathFinderMap(const int in_width,
			const int in_height,
			const uint8_t in_invalid_cell_data = 0,
			const std::vector<uint8_t>& in_data = {}
			);
		const uint8_t GetCell(const VectorShort2& in_location) const;
		void SetCell(const VectorShort2& in_location, const uint8_t in_data);

		//void AddChangeCallback(const TMapChangeCallback& in_callback, const uint8_t in_dirty_mask = 0xFF);
		void AddObserver(const std::shared_ptr<IPathFinderMapObserver>& in_observer, const uint8_t in_dirty_mask = 0xFF);
		void RemoveObserver(IPathFinderMapObserver* const in_observer);

		const int GetWidth() const { return _width; }
		const int GetHeight() const { return _height; }

	private:
		/// return true for being a valid offset?
		static const bool MakeOffset(
			int32_t& out_offset,
			const int32_t in_width,
			const int32_t in_height,
			const VectorShort2& in_location
			);

	private:
		const int _width;
		const int _height;
		const uint8_t _invalid_cell_data;
		std::vector<uint8_t> _data;

		/// cache mask and raw pointer
		std::vector<std::pair<uint8_t, IPathFinderMapObserver*>> _dirty_mask_observer_array;
		/// ownership
		std::vector<std::shared_ptr<IPathFinderMapObserver>> _observer_array;
	};
};