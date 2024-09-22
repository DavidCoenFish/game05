#include "unit_test_pch.h"
#include "common/math/vector_float3.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonMathVectorFloat3
{
	TEST_CLASS(PublicMethods)
	{
	public:
		TEST_METHOD(CrossTest)
		{
            const auto cross = Cross(VectorFloat3(1.0f, 0.0f, 0.0f), VectorFloat3(0.0f, 1.0f, 0.0f));
            Assert::AreEqual(cross[0], 0.0f, 0.0000001f);
            Assert::AreEqual(cross[1], 0.0f, 0.0000001f);
            Assert::AreEqual(cross[2], 1.0f, 0.0000001f);
		}

	};
}
