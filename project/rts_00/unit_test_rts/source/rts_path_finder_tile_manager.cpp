#include "unit_test_rts_pch.h"
#include "static_rts/path_finder/path_finder_map.h"
#include "static_rts/path_finder/path_finder_tile_manager.h"

#include "common/math/vector_short2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	const bool IsMapDataTraversable(const uint8_t in_data)
	{
		return 0 != (in_data & 0x01);
	}
}

namespace RtsPathFinderTileManager
{
	TEST_CLASS(PublicMethods)
	{
	public:
		TEST_METHOD(TestCtor)
		{
			StaticRts::PathFinderMap map(78, 103);

			std::shared_ptr<StaticRts::PathFinderTileManager> tile_manager = std::make_shared<StaticRts::PathFinderTileManager>(IsMapDataTraversable);

			map.AddObserver(tile_manager, 0x01);
		}

		TEST_METHOD(TestRegion0)
		{
			std::vector<uint8_t> data({
				1,1,1,1,1,1,1,1, 1,1,1,1,0,1,1,1, 1,1,1,1,0,1,1,1, 1,1,1,1,1,1,1,1,
				1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
				1,0,1,1,1,1,1,0, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,
				1,0,1,0,0,0,1,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 1,0,0,0,0,1,0,1,
				1,0,1,0,1,0,1,0, 0,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,0, 1,0,1,1,0,1,0,1,
				1,0,1,1,1,0,1,1, 1,1,0,1,1,0,1,1, 1,1,0,1,1,0,1,1, 1,0,1,1,1,1,0,1,
				1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,1,
				1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
				});

			StaticRts::PathFinderMap map(32, 8, 0, data);
			std::shared_ptr<StaticRts::PathFinderTileManager> tile_manager = std::make_shared<StaticRts::PathFinderTileManager>(IsMapDataTraversable);
			map.AddObserver(tile_manager, 0x01);

			Assert::AreEqual(tile_manager->IsSameRegion(
				VectorShort2(0,0),
				VectorShort2(10,4)
				), true);

			Assert::AreEqual(tile_manager->IsSameRegion(
				VectorShort2(10,4),
				VectorShort2(0,0)
				), true);

			Assert::AreEqual(tile_manager->IsSameRegion(
				VectorShort2(10,4),
				VectorShort2(17,0)
				), false);
		}

	};
}
