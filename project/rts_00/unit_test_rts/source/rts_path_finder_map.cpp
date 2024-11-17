#include "unit_test_rts_pch.h"
#include "static_rts/path_finder/path_finder_map.h"
#include "static_rts/path_finder/i_path_finder_map_objserver.h"

#include "common/math/vector_short2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
class TestMapObserver : public StaticRts::IPathFinderMapObserver
{
public:
	TestMapObserver(int32_t& in_count) : _count(in_count)
	{
		//nop
	}
private:
	virtual void OnChange(const VectorShort2& in_location, const uint8_t in_data) override
	{
		_count += 1;
	}

private:
	int32_t& _count;
};

}


namespace RtsPathFinderMap
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
		}

		TEST_METHOD(TestCallbacks)
		{
			const int32_t width = 78;
			const int32_t height = 103;
            StaticRts::PathFinderMap map(width, height);

			int32_t call_count = 0;
			std::shared_ptr<TestMapObserver> observer = std::make_shared<TestMapObserver>(call_count);
			map.AddObserver(observer);
			Assert::AreEqual(call_count, 0);

			map.SetCell(VectorShort2(1,3), 7);
			Assert::AreEqual(call_count, 1);

			map.SetCell(VectorShort2(1,3), 7);
			Assert::AreEqual(call_count, 1);

			map.SetCell(VectorShort2(1,3), 8);
			Assert::AreEqual(call_count, 2);

			map.SetCell(VectorShort2(0,0), 1);
			Assert::AreEqual(call_count, 3);

			// presumtion default invalid value is 0, and no data change results in no broadcast
			map.SetCell(VectorShort2(1,1), 0);
			Assert::AreEqual(call_count, 3);

		}
		TEST_METHOD(TestCallbacksFilter)
		{
			const int32_t width = 78;
			const int32_t height = 103;
            StaticRts::PathFinderMap map(width, height);

			int32_t call_count = 0;
			std::shared_ptr<TestMapObserver> observer = std::make_shared<TestMapObserver>(call_count);
			map.AddObserver(observer, 0x01);
			Assert::AreEqual(call_count, 0);

			map.SetCell(VectorShort2(1,3), 7);
			Assert::AreEqual(call_count, 1);

			map.SetCell(VectorShort2(1,8), 4);
			Assert::AreEqual(call_count, 1);

			map.SetCell(VectorShort2(1,8), 5);
			Assert::AreEqual(call_count, 2);

		}
	};
}
