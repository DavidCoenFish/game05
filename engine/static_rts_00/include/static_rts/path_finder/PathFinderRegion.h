#pragma once

#include "Common/PathFinder/PathFinderLocation.h"

namespace PathFinder
{
	class Region
	{
	public:
		class RegionStepData
		{
		public:
			RegionStepData(const int edgeMask)
				: m_edgeMask(edgeMask)
			{
				return;
			}
			int m_edgeMask = 0;
			int m_regionIDofEdgeMaskDestination[32] = {};
		};

		Region(
			const int pathManagerChangeID,
			const std::map< int, std::shared_ptr<Region::RegionStepData> >& mapRegionKeyStepData
			);

		const int GetPathManagerChangeID() const;
		void SetRegionKeyStepData( const int regionKey, const std::shared_ptr< RegionStepData >& pStepData );

	private:
		int m_pathManagerChangeID; //don't need to regenerate a region if it has the same changeid as the path manager
		std::map<int, std::shared_ptr< RegionStepData >> m_mapRegionKeyStepData;

	};
};