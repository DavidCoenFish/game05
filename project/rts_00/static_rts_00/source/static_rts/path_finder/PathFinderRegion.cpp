#include "CommonPCH.h"

#include "Common/PathFinder/PathFinderRegion.h"


PathFinder::Region::Region(
	const int pathManagerChangeID,
	const std::map< int, std::shared_ptr<Region::RegionStepData> >& mapRegionKeyStepData
	)
	: m_pathManagerChangeID(pathManagerChangeID)
	, m_mapRegionKeyStepData(mapRegionKeyStepData)
{
	return;
}

const int PathFinder::Region::GetPathManagerChangeID() const
{
	return m_pathManagerChangeID;
}

void PathFinder::Region::SetRegionKeyStepData( const int regionKey, const std::shared_ptr< RegionStepData >& pStepData )
{
	m_mapRegionKeyStepData[regionKey] = pStepData;
	return;
}
