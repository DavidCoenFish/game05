#pragma once

#include "static_rts/path_finder/i_path_finder_path.h"
#include "common/math/vector_short2.h"

namespace StaticRts
{
	/// deal with the cose of a path with the same start and end location
	class PathFinderPathDataCoincident : public IPathFinderPathData
	{
	public:
		PathFinderPathDataCoincident(
			const VectorShort2& in_location
			);

	private:
		virtual const VectorShort2 GetCurrentLocation() const override;
		virtual const bool StepLocation() override;
		virtual const bool LookAheadLocation(
			VectorShort2& out_location, 
			const int32_t in_offset
			) override;

	private:
		VectorShort2 _location;

	};
};