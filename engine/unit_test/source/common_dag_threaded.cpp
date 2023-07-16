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
			auto collection = DagThreadedCollection::Factory();
			Assert::IsNotNull( collection.get() );
			collection.reset();
		}

		TEST_METHOD(DagThreadedFindNull)
		{
			auto collection = DagThreadedCollection::Factory();
			Assert::IsNotNull( collection.get() );
			auto node_found = collection->FindNode("bar");
			Assert::IsNull(node_found.get());
		}

		TEST_METHOD(DagThreadedCreateVariable)
		{
			auto collection = DagThreadedCollection::Factory();
			Assert::IsNotNull( collection.get() );
			auto node = collection->CreateNodeVariable("foo");
			Assert::IsNotNull(node.get());
			auto node_found = collection->FindNode("foo");
			Assert::AreEqual((void*)node.get(), (void*)node_found.get());
			auto dag_value = DagThreadedHelper::CreateDagValue<int>(3);
			collection->SetDagValue(node, dag_value);

			auto found_dag_value = collection->GetDagValue(node);
			Assert::IsNotNull(found_dag_value.get());
			const int value_out = DagThreadedHelper::GetValue<int>(found_dag_value);
			Assert::AreEqual(value_out, 3);
		}

		TEST_METHOD(DagThreadedCreateCalculate)
		{
			auto collection = DagThreadedCollection::Factory();
			Assert::IsNotNull( collection.get() );

			auto node_input_0 = collection->CreateNodeVariable("i0", DagThreadedHelper::CreateDagValue<int>(3));
			auto node_input_1 = collection->CreateNodeVariable("i1", DagThreadedHelper::CreateDagValue<int>(4));

			auto node_calculate = collection->CreateNodeCalculate("c", [](
				const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_stack, 
				const std::vector< std::shared_ptr< IDagThreadedValue > >&
				)-> std::shared_ptr< IDagThreadedValue > {
				int sum = 0;
				for (auto iter: in_array_stack)
				{
					sum += DagThreadedHelper::GetValue<int>(iter);
				}
				return DagThreadedHelper::CreateDagValue<int>(sum);
			});

			{
				const int value_out = DagThreadedHelper::GetValue<int>(collection->GetDagValue(node_calculate));
				Assert::AreEqual(value_out, 0);
			}

			collection->AddNodeLinkStack(node_calculate, node_input_0);
			collection->AddNodeLinkStack(node_calculate, node_input_1);

			{
				const int value_out = DagThreadedHelper::GetValue<int>(collection->GetDagValue(node_calculate));
				Assert::AreEqual(value_out, 7);
			}
			return;
		}


		TEST_METHOD(DagThreadedExceptionOnDuplicateNameNode)
		{
			auto lambda = []()
			{
				DagThreadedCollection* collection = new DagThreadedCollection();
				Assert::IsNotNull( collection );
				collection->CreateNodeCalculate(std::string("foo"), nullptr);
				collection->CreateNodeCalculate(std::string("foo"), nullptr);
			};

			Assert::ExpectException<std::exception>(lambda);
		}
	};
}
