#pragma once

#include "static_rts/path_finder/i_path_finder_map_observer.h"

class PathFinderTileManagerImplementation;
class VectorShort2;

namespace StaticRts
{
	class IPathFinderPath;

	/// cut the input map into a sersies of tiles of traversable [true,false] and watch for changes
	/// as requires, calculate the flow regions, and use that to construct a path
	/// tile manager and tiles have a change id? and on use of path intance, i needs to check if still valid?
	/// otherwise tiles have a list of paths that reference them, so on tile change to being dirty, they mark the path as dirty
	/// hen path recaculates, pass in the set of tile that was in use, and get a new set of tile? change id may be better?
	class PathFinderTileManager : public IPathFinderMapObserver
	{
	public:
		typedef const bool (*TIsMapDataTraversable)(const uint8_t in_data);

		PathFinderTileManager(
			const TIsMapDataTraversable in_is_data_traversable_function
			);
		~PathFinderTileManager();

		/// return true if point a and b are in the same region
		/// returns false if a or b are invalid locations (non traversable)
		const bool IsSameRegion(const VectorShort2& in_a, const VectorShort2& in_b);

		/// we may not actually be able to reach the target location, and path may need to be recalculated after map change, but after initialsation,
		/// the RefreashPath should deal with that. create a new path object and call InitialisePath for target change? else allow IPathFinderPath to reset? or just call InitialisePath again?
		void InitialisePath(
			IPathFinderPath& in_path, 
			const VectorShort2& in_current_location, 
			const VectorShort2& in_target_location
			);

		/// update (also allow implimentation to advance location on) the path, return false if path traversal finished
		const bool RefreashPath(IPathFinderPath& in_path);

	private:
		virtual void SetInitialData(
			const int32_t in_width,
			const int32_t in_height,
			const std::vector<uint8_t>& in_data
			) override;

		virtual void OnChange(const VectorShort2& in_location, const uint8_t in_data) override;

	private:
		std::unique_ptr<PathFinderTileManagerImplementation> _implementation;

	};
};