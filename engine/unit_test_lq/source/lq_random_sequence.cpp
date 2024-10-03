#include "unit_test_lq_pch.h"
#include "static_lq\random_sequence.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LqRandomSequence
{
	TEST_CLASS(PublicMethods)
	{
	public:
		TEST_METHOD(TestSequence)
		{
            StaticLq::RandomSequence randomSequence00(42);
            StaticLq::RandomSequence randomSequence01(42);

            for (int index = 0; index < 100; ++index)
            {
			    Assert::AreEqual(randomSequence00.GenerateFloat(), randomSequence01.GenerateFloat());
            }
		}

		TEST_METHOD(TestFloat)
		{
            StaticLq::RandomSequence randomSequence00(107);
            for (int index = 0; index < 100; ++index)
            {
                const float value = randomSequence00.GenerateFloat();
			    Assert::IsTrue(0.0f <= value);
			    Assert::IsTrue(value < 1.0f);
            }
		}

		TEST_METHOD(TestSeed)
		{
            StaticLq::RandomSequence randomSequence00(0);
            Assert::AreEqual(randomSequence00.GetSeed(), static_cast<uint32_t>(0));

            StaticLq::RandomSequence randomSequence01(42);
            Assert::AreEqual(randomSequence01.GetSeed(), static_cast<uint32_t>(42));

            bool different = false;
            for (int index = 0; index < 100; ++index)
            {
                if (randomSequence00.GenerateFloat() != randomSequence01.GenerateFloat())
                {
                    different = true;
                    break;
                }
            }
            Assert::IsTrue(different);
		}

		TEST_METHOD(TestDistributionDice)
		{
            StaticLq::RandomSequence randomSequence00(42);
            int result_count[4] = {0,0,0,0};
            for (int index = 0; index < 10000; ++index)
            {
                const int result = randomSequence00.GenerateDice(4);
                Assert::IsTrue(0 < result);
                Assert::IsTrue(result < 5);
                result_count[result - 1] += 1;
            }

            for (int index = 0; index < 4; ++index)
            {
                Assert::IsTrue(2000 < result_count[index]);
                Assert::IsTrue(result_count[index] < 3000);
            }
        }
		TEST_METHOD(TestDistributionFloat)
		{
            StaticLq::RandomSequence randomSequence00(42);
            float sum_value = 0.0f;
            float sum_distance_middle = 0.0f;
            for (int index = 0; index < 10000; ++index)
            {
                const float value = randomSequence00.GenerateFloat();
                sum_value += value;
                const float distance_middle = abs(value - 0.5f);
                sum_distance_middle += distance_middle;
            }

            const float average = sum_value / static_cast<float>(10000);
            const float average_distance_middle = sum_distance_middle / static_cast<float>(10000);

            Assert::IsTrue(0.475f < average);
            Assert::IsTrue(average < 0.525f);
            Assert::IsTrue(0.225f < average_distance_middle);
            Assert::IsTrue(average_distance_middle < 0.275f);
        }

	};
}
