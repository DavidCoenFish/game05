#include "unit_test_lq_pch.h"
#include "static_lq_00\random_sequence.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LqRandomSequence
{
	TEST_CLASS(PublicMethods)
	{
	public:
		TEST_METHOD(DagThreadedCollectionCreation)
		{
			//Logger::WriteMessage(L"Hello world");
			auto collection = DagThreadedCollection::Factory();
			Assert::IsNotNull( collection.get() );
			collection.reset();
		}
	};
}
