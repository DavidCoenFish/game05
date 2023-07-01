#include "unit_test_pch.h"
#include "common\dag_threaded\dag_threaded_collection.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonDagThreaded
{
	TEST_CLASS(PublicMethods)
	{
	public:
		TEST_METHOD(DagThreadedCollectionCreation)
		{
			//Logger::WriteMessage(L"Hello world");
			auto pCollection = DagThreadedCollection::Factory();
			Assert::IsNotNull( pCollection.get() );
			pCollection->SetMultiThreadedMode(true);
			pCollection->SetMultiThreadedMode(false);
			pCollection.reset();
		}
		TEST_METHOD(DagThreadedCollectionCreationException)
		{
			auto lambda = []()
			{
				DagThreadedCollection* pCollection = new DagThreadedCollection();
				Assert::IsNotNull( pCollection );
				pCollection->SetMultiThreadedMode(true);

				pCollection->CreateNodeCalculate(std::string("foo"), nullptr);
			};

			Assert::ExpectException<std::exception>(lambda);
		}
	};
}
