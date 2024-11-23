#include "static_rts/static_rts_pch.h"
#include "static_rts/path_finder/path_finder_path_data_coincident.h"

StaticRts::PathFinderPathDataCoincident::PathFinderPathDataCoincident(
	const VectorShort2& in_location
	)
	: _location(in_location)
{
	//nop
}

const VectorShort2 StaticRts::PathFinderPathDataCoincident::GetCurrentLocation() const
{
	return _location;
}

const bool StaticRts::PathFinderPathDataCoincident::StepLocation()
{
	return false;
}
const bool StaticRts::PathFinderPathDataCoincident::LookAheadLocation(
	VectorShort2&, 
	const int32_t
	)
{
	return false;
}
