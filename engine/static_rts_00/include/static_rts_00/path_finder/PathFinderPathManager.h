#pragma once

#include "Common/PathFinder/PathFinderLocation.h"
#include "Common/PathFinder/PathFinderPathTile.h"
#include "Common/PathFinder/PathFinderPathInstance.h"
#include "Common/PathFinder/PathFinderMap.h"
#include "Common/PathFinder/PathFinderRegion.h"

/*
get an array of Locations to get from one location to another
 - but how to know when it is invalidated? get a path instance which we move along?
 - if start and end locations are on the same region, then we just get the array of locations between
 - if start and end locations are NOT on the same region, move as far as we can in the start region

Region
	unique id (and we keep on incrementing on making each now region?)
	vector [ PathTile index, region id, change id ]
	?best points for cardinal directions, to go to if we can't get to destination

how to get from a change to a tile, to correctly recalculating the region
how to calculate the regions~ flow across path tiles? get regions 
how to reduce region thrashing on small changes? may not be that bad? 
or do we have a PathTile region change id incremented if nb of regions or edge masks change?

Path
	region id
	vector [location]

make a 32bit flag for which edge cells hold a given region? ie, each PathTile region has a edge mask?
Region has a "set"/"map" of <path tile index, path tile region id> to know 

//for a path to still be valid, it needs to use traversalChangeID
//for a region to still be valid, it could just use regionEdgeMaskChangeId
traversalChangeID, rather than region change id or edge change id. 

PathManager::map<tile offset:region id, tileTraversalChangeID:Region>



so, i want to generate a path from one location to another
  return a path instance with the info to check if path valid, and generate array of location steps
  get the tile and region for start
  get the tile and region for destination
   if same tile and region, map path instance. uses transition change id of tile to make path
  use region cache to find path to destination or next closest point

region cache, 


*/

namespace PathFinder
{
	class PathInstance;

	template <
		typename CELL_DATA, 
		const bool (&FCELL_DATA_TRAVERSABLE)(const CELL_DATA&)
		>
	class PathManager
	{
	public:
		constexpr int s_dim = 8;
		typedef PathTile<s_dim, s_dim, char> TPathTile;
		typedef std::bitset< (((s_dim + 1) * (s_dim + 1)) - 1)> TBitArray;
		typedef Map<CELL_DATA> TMap;

		PathManager(TMap& map)
			: m_width(0)
			, m_height(0)
			, m_changeId(0)
		{
			const int mapWidth = map.GetWidth();
			m_width = PadUpValue(mapWidth);
			const int mapHeight = map.GetHeight();
			m_height = PadUpValue(mapHeight);
			m_tileArray.reserve(m_width * m_height);

			for (int tileY = 0; tileY < m_height; ++tileY)
			{
				for (int tileX = 0; tileX < m_width; ++tileX)
				{
					TBitArray data;
					bool traversable = false;

					for (int traceY = 0; traceY <= s_dim; ++traceY)
					{
						for (int traceX = 0; traceX <= s_dim; ++traceX)
						{
							int offset = traceX + (traceY * (s_dim + 1));
							int locationX = (tileX * s_dim) + traceX;
							if (mapWidth < locationX)
							{
								continue;
							}
							int locationY = (tileY * s_dim) + traceY;
							if (mapHeight < locationY)
							{
								continue;
							}

							data[offset] = map.Query(PackLocation(locationX, locationY));
						}
					}

					m_tileArray.push_back(TPathTile(
						PackLocation(tileX * s_dim, tileY * s_dim),
						data
						));
				}
			}

			map.AddChangeCallback([](PASS_LOCATION_CONST location, const CELL_DATA& data){
				Change(location, data);
				});
		}

		const int GetChangeId() const { return m_changeId; }
		 
		std::shared_ptr< PathInstance > GeneratePathInstance(PASS_LOCATION_CONST start, PASS_LOCATION_CONST end)
		{
			//how to deal with move request to same location?
			if (start == end)
			{
				return nullptr;
			}

			const int startOffset = GetTileOffset(start);
			const int startRegion = m_tileArray[startOffset].GetRegion(start);
			if (0 == startRegion)
			{
				//no path if not on traverable tile
				return nullptr;
			}

			const int endOffset = GetTileOffset(end);
			const int endRegion = m_tileArray[endOffset].GetRegion(end);
			//if we start and end on the same tile and region, make a simple path instance
			if ((startOffset == endOffset) && (startRegion == endRegion))
			{
				std::vector< Location > path;
				path.push_back(start);
				m_tileArray[startOffset].AppendPath(start, end, path);
				const int traversalChangeID = m_tileArray[startOffset].GetTraversalChangeID();
				return PathInstance::FactorySimple(start, end, path, startOffset, traversalChangeID); //m_changeId);
			}

			const std::shared_ptr<Region> pRegion = GetRegion(startOffset, startRegion);
			return PathInstance::FactoryRegion(pRegion, start, startRegion, end, endRegion);
		}

		//return false if traversal finished (reached end, no more move, not inside traversable tile
		//return true if move is still left
		const bool RefreashPathInstance(
			PathInstance& pathInstance
			)
		{
			pathInstance;
			return false;
		}

	private:
		void Change(PASS_LOCATION_CONST location, const CELL_DATA& data)
		{
			const int offset = GetTileOffset(location);
			const bool traversable = FCELL_DATA_TRAVERSABLE(data);
			bool change = false;

			if (true == m_tileArray[offset].Change(location, traversable))
			{
				change = true;
			}

			//deal with tile location overlap
			int overlapOffsetA = 0;
			int overlapOffsetB = 0;
			if (true == GetOverlapTileOffset(location, overlapOffsetA, overlapOffsetB))
			{
				if (-1 != overlapOffsetA)
				{
					if (true == m_tileArray[overlapOffsetA].Change(location, traversable))
					{
						change = true;
					}
				}
				if (-1 != overlapOffsetB)
				{
					if (true == m_tileArray[overlapOffsetB].Change(location, traversable))
					{
						change = true;
					}
				}
			}

			if (true == change)
			{
				m_changeId += 1;
			}

			return;
		}

		const int PadUpValue(const int value)
		{
			int result = value / s_dim;
			if (0 != (value - (result * s_dim)))
			{
				result += 1;
			}
			return result;
		}

		const bool IsTileCoordValid(const int tileX, const int tileY)
		{
			return (
				(0 <= tileX) && 
				(tileX < m_width) && 
				(0 <= tileY) && 
				(tileY < m_height)
				);
		}

		const int GetTileOffset(PASS_LOCATION_CONST location)
		{
			short x = 0;
			short y = 0;
			UnPackLocation(location, x, y);
			const int tileX = x / s_dim;
			const int tileY = y / s_dim;
			DSC_ASSERT(true == IsTileCoordValid(tileX, tileY));
			const int offset = tileX + (tileY * m_width);
			return offset;
		}
		void UnpackTileOffset(const int tileOffset, int& x, int& y)
		{
			y = tileOffset / m_width;
			x = tileOffset - (y * m_width);
			return;
		}

		const bool GetOverlapTileOffset(PASS_LOCATION_CONST location, int& overlapOffsetA, int& overlapOffsetB)
		{
			short x = 0;
			short y = 0;
			UnPackLocation(location, x, y);
			const int tileX = x / s_dim;
			const int tileY = y / s_dim;
			bool result = false;
			overlapOffsetA = -1;
			overlapOffsetB = -1;
			if ((0 == (x % s_dim)) && (1 < tileX))
			{
				result = true;
				overlapOffsetA = (tileX - 1) + (tileY * m_width)
			}
			if ((0 == (y % s_dim)) && (1 < tileY))
			{
				result = true;
				overlapOffsetB = tileX + ((tileY - 1) * m_width);
			}

			return result;
		}

		static const int PackTileOffsetRegion(const int tileOffset, const int regionID)
		{
			//not interested in region 0? or is that not the regard of the data packer
			DSC_ASSERT((0 <= regionID) && (regionID <= 0xFF));
			DSC_ASSERT((0 <= tileOffset) && (tileOffset <= 0xFFFFFF));
			return ((tileOffset << 8) | (regionID & 0xFF));
		}

		const std::shared_ptr<Region> GetRegion(const int tileOffset, const int regionID)
		{
			const int regionKey = PackTileOffsetRegion(tileOffset, regionID);
			const auto found = m_mapRegion.find(regionKey);
			if ((found != m_mapRegion.end()) && (m_changeID == found.second->GetPathManagerChangeID()))
			{
				return found.second;
			}

			std::map< int, std::shared_ptr<Region::RegionStepData> > setRegionKey;
			//setRegionKey.push(regionKey);

			//flow region
			FlowRegion(setRegionKey, tileOffset, regionID, regionKey);

			std::shared_ptr<Region> pResult = std::make_shared<Region>(m_changeID, setRegionKey);
			for (const auto& iter : setRegionKey)
			{
				m_mapRegion[iter] = pResult;
			}

			return pResult;
		}

		//flow region
		void FlowRegion(std::map< int, std::shared_ptr<Region::RegionStepData> >& setRegionKey, const int tileOffset, const int regionID, const int regionKey)
		{
			if (setRegionKey.find(regionKey) != setRegionKey.end())
			{
				return;
			}
			const int tileMask = m_tileArray[tileOffset].GetRegionMask();
			int x = 0;
			int y = 0;
			UnpackTileOffset(tileOffset, x, y);
			auto pStepData = std::make_shared< Region::RegionData >(tileMask);
			setRegionKey[regionKey] = pStepData;

			int trace = 0;
			constexpr TOffset offset[TOffsetCount] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
			for( int index = 0; index < TOffsetCount; ++index)
			{
				const int tileX = (x + offset[index].m_x);
				const int tileY = (y + offset[index].m_y);

				if (true == IsTileCoordValid(vistX / s_dim, vistY / s_dim))
				{
					for (int index = 0; index < s_dim; ++index)
					{
						if (0 == (tileMask & (1 << index)))
						{
							continue;
						}

						const Location innerLocation = m_tileArray[tileOffset].MakeLocationForBit(trace);
						const int innerOffset = tileX + (tileY * m_width)
						const int innerRegionID = m_tileArray[innerOffset].GetRegion(innerLocation);
						pStepData->m_regionIDofEdgeMaskDestination[trace] = innerRegionID;

						trace += 1;

						if (0 == innerRegionID)
						{
							continue;
						}

						const int innerRegionKey = PackTileOffsetRegion(innerOffset, innerRegionID);
						FlowRegion(setRegionKey, innerOffset, innerRegionID, innerRegionKey);
					}
				}
				else
				{
					trace += s_dim;
				}
			}
		}

	private:
		std::vector< TPathTile > m_tileArray;
		int m_width;
		int m_height;

		int m_changeID; //don't need to regenerate a region if it has the same changeid as the path manager

		std::map< int, std::shared_ptr< Region > > m_mapRegion;


	};
};