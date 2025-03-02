#include "unit_test_pch.h"
#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"

#include <iostream>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StdIfstream
{
	TEST_CLASS(PublicMethods)
	{
	public:
		TEST_METHOD(OpenFile)
		{
			std::string filename("./data/test.txt");
			std::ifstream infile(filename.c_str());
			Assert::IsTrue(infile.good());
		}

	};
}
