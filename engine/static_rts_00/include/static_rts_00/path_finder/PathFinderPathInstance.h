#pragma once

#include "Common/PathFinder/PathFinderLocation.h"
#include "Common/Macro.h"

namespace PathFinder
{
	class PathInstance
	{
	public:
		static std::shared_ptr<PathInstance> FactorySimple(
			PASS_LOCATION_CONST start, 
			PASS_LOCATION_CONST end, 
			std::vector< Location >& path, 
			const int pathManagerTileIndex, 
			const int pathTileTraversalChangeID
			)
		{
		}

		PathInstance();

		/*
		move this under PathManager so we don't need to know of template param
		*/
		//return false if traversal finished (reached end, can more no further, not inside traversable tile
		//return true if move is still left
		//const bool UpdateValidity(
		//	PathManager
		//	);
		//const bool ReachedEnd() const;
		//const bool CanReachedEnd() const; //is the end in the same region as the current location
		//const bool NoMoreMove() const;


	private:

	};
};