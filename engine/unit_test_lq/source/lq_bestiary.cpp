#include "unit_test_lq_pch.h"
#include "static_lq\bestiary\bestiary.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LqBestiary
{
	TEST_CLASS(StaticMethods)
	{
	public:
		TEST_METHOD(FactoryDefaultGiantSpider)
		{
            auto item = static_lq::Bestiary::FactoryDefaultGiantSpider();
			Assert::IsNotNull(item.get());
		}

		TEST_METHOD(FactoryDefaultGiantAnt)
		{
            auto item = static_lq::Bestiary::FactoryDefaultGiantAnt();
			Assert::IsNotNull(item.get());
		}

	};
}
