#pragma once

#include "static_rts/path_finder/i_path_finder_path.h"

class VectorShort2;


namespace StaticRts
{
	class PathFinderPathData : public IPathFinderPathData
	{
	public:
		struct RegionStepData
		{
			const int32_t _tile_index = 0;
			const uint8_t _tile_sub_region_id = 0;
			VectorShort2 _location_step_start = {};
		};
		struct StepData
		{
			/// the tile/region index, or an index into _stack_region_steps 
			//const int32_t _tile_index = 0;
			//const uint8_t _tile_sub_region_id = 0;
			const int32_t _stack_index = 0;
			VectorShort2 _location = {};
		};

		PathFinderPathData(
			const int32_t in_tile_manager_change_id,
			const std::vector<RegionStepData>& in_stack_region_steps,
			const std::vector<StepData>& in_stack_region_steps
			);

	private:
		virtual const VectorShort2 GetCurrentLocation() const override;
		virtual const bool StepLocation() override;
		virtual const bool LookAheadLocation(
			VectorShort2& out_location, 
			const int32_t in_offset
			) override;

	private:
		int32_t _tile_manager_change_id = 0;
		std::vector<RegionStepData> _stack_region_steps;
		/// back of step data should be the current location
		std::vector<StepData> _stack_region_steps;
	};
};