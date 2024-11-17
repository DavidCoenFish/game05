#pragma once

#include "static_rts/path_finder/i_path_finder_map_objserver.h"

class PathFinderPath;
class PathFinderTileManagerImplementation;
class VectorShort2;

namespace StaticRts
{
	class PathFinderTileManager : public IPathFinderMapObserver
	{
	public:
		typedef const bool (*TIsMapDataTraversable)(const uint8_t in_data);

		PathFinderTileManager();

		std::shared_ptr<PathFinderPath> MakePath(const VectorShort2& in_start, const VectorShort2& in_end);

	private:
		virtual void OnChange(const VectorShort2& in_location, const uint8_t in_data) override;

	private:
		std::unique_ptr< PathFinderTileManagerImplementation > _implementation;
	};
};