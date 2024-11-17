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
			const int32_t width = 78;
			const int32_t height = 103;
			StaticRts::PathFinderMap map(width, height);

			Assert::AreEqual(width, map.GetWidth());
			Assert::AreEqual(height, map.GetHeight());

			std::shared_ptr<StaticRts::PathFinderTileManager> tile_manager = std::make_shared<StaticRts::PathFinderTileManager>(IsMapDataTraversable);

			map.AddObserver(tile_manager, 0x01);
		}
	};
}
