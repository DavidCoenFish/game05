#pragma once

class VectorShort2;

namespace StaticRts
{
	/// data for the path finder system to hold some arbitary data for a path, but 
	class IPathFinderPathData
	{
	public:
		virtual ~IPathFinderPathData(){}

		/// have IPathFinderPathData own current location, IPathFinderPath can own historic location if it wants
		virtual const VectorShort2 GetCurrentLocation() const = 0;
		/// advance current location on path to the next location, return false if there was no next
		virtual const bool StepLocation() = 0;

		/// in_offset == 0, get current location, or prefer usage of GetCurrentLocation
		/// in_offset < 0 not supported?
		/// in_offset == 1, get next location...
		/// return false if location requested not valid/ past end of path
		virtual const bool LookAheadLocation(
			VectorShort2& out_location, 
			const int32_t in_offset
			) = 0;
	};

	//class IPathFinderPathDataSource
	//{
	//public:
	//	/// may need to generate/regenerate PathFinderPathData
	//	/// return false if current location has become invalid/ crushed by gate?
	//	virtual const bool RefreshData(
	//		int32_t& in_out_tile_manager_change_id,
	//		const VectorShort2& in_target_location,
	//		std::shared_ptr<IPathFinderPathData>& in_out_path_data
	//		) = 0;
	//};

	/// want to allow systems external to specialise how they advance on a path, but still want to know from the path finder system
	/// the recomended sequence of cells to travers to get from one location to another
	/// need to check path is still valid, need to check current location is still traverable (unit destroyed by closing gate?)
	/// we may want more than one cell in advance/ making going around coreners more natural
	/// 
	class IPathFinderPath
	{
	public:
		/// IPathFinderPathData has been updated to be valid for current state of path finder data, and ready for IPathFinderPath use
		/// return false when path traversal finished
		/// could pass in IPathFinderPathData& in_path_data to make it explicit that it is now avaliable for use, but IPathFinderPath is already being asked to own it
		virtual const bool Update(IPathFinderPathData& in_path_data) = 0;
		/// use IPathFinderPath to hold PathFinder system's data
		virtual void SetPathData(const std::shared_ptr<IPathFinderPathData>& in_path_data) = 0;
		virtual IPathFinderPathData& GetPathData() = 0;
	};
}