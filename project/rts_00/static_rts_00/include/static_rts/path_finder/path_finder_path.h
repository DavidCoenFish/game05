#pragma once

class VectorShort2;

namespace StaticRts
{
	/// 
	class PathFinderPath
	{
	public:
		PathFinderPath();
	private:
		PathFinderTileManager& _parent;

		int32_t _tile_manager_change_id = 0;
		struct TData
		{
			int32_t _tile_id; // index
			int32_t _tile_change_id;
		};
		std::vector<TData> _dependency_array = {};

		struct TStep
		{
			int32_t _tile_id = 0; // index
			int32_t _tile_sub_region_id = 0;
			VectorShort2 _start = {};
			VectorShort2 _end = {};
		};
		std::vector<TStep> _step_array = {};

		VectorShort2 _current_location = {};
		VectorShort2 _next_location = {};
		float _progress_ratio = 0.0f;
		VectorShort2 _target_location = {};
	};
};