#include "unit_test_pch.h"
#include "common\dag_threaded\dag_threaded_collection.h"
#include "common\dag_threaded\dag_threaded_helper.h"

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
		TEST_METHOD(DagThreadedExceptionOnCreateCalculate)
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
		TEST_METHOD(DagThreadedFindNull)
		{
			auto pCollection = DagThreadedCollection::Factory();
			Assert::IsNotNull( pCollection.get() );
			auto node_found = pCollection->FindNode("bar");
			Assert::IsNull(node_found);
		}

		TEST_METHOD(DagThreadedCreateVariable)
		{
			auto pCollection = DagThreadedCollection::Factory();
			Assert::IsNotNull( pCollection.get() );
			auto node = pCollection->CreateNodeVariable("foo");
			Assert::IsNotNull(node);
			auto node_found = pCollection->FindNode("foo");
			Assert::AreEqual(node, node_found);
			auto dag_value = pCollection->CreateDagValue<int>(3);
			pCollection->SetDagValue(node, dag_value);

			auto found_dag_value = pCollection->GetDagValue(node);
			Assert::IsNotNull(found_dag_value.get());
			const int value_out = DagThreadedHelper::GetValue<int>(found_dag_value);
			Assert::AreEqual(value_out, 3);
		}
	};
}
