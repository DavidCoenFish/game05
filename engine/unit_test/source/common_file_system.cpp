#include "unit_test_pch.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonFileSystem
{
	TEST_CLASS(Sanity)
	{
		public:
		TEST_METHOD(SyncReadFile)
		{
			FileSystem::TFileData data = FileSystem::SyncReadFile("./data/test.txt");
			Assert::IsNotNull(data.get());
			Assert::IsTrue(0 != data->size());
			Log::RunningUintTest();
		}

		TEST_METHOD(SyncReadFileR)
		{
			FileSystem::TFileData data = FileSystem::SyncReadFile("./data/screen_quad_vertex.cso");
			Assert::IsNotNull(data.get());
			Assert::IsTrue(0 != data->size());
			Log::RunningUintTest();
		}

		TEST_METHOD(SyncReadFileRLong)
		{
			FileSystem::TFileData data = FileSystem::SyncReadFile("./data/screen_quad_vertex_012345678901234567890123456789012345678901234567890.cso");
			Assert::IsNotNull(data.get());
			Assert::IsTrue(0 != data->size());
			Log::RunningUintTest();
		}

		//TEST_METHOD(SyncReadFileRA)
		//{
		//	FileSystem::TFileData data = FileSystem::SyncReadFile("G:/development/game05/build/engine/unit_test/x64/Debug/output/data/screen_quad_vertex.cso");
		//	Assert::IsNotNull(data.get());
		//	Assert::IsTrue(0 != data->size());
		//	Log::RunningUintTest();
		//}

		//TEST_METHOD(SyncReadFileRALong)
		//{
		//	FileSystem::TFileData data = FileSystem::SyncReadFile("G:/development/game05/build/engine/unit_test/x64/Debug/output/data/screen_quad_vertex_012345678901234567890123456789012345678901234567890.cso");
		//	Assert::IsNotNull(data.get());
		//	Assert::IsTrue(0 != data->size());
		//	Log::RunningUintTest();
		//}

		//TEST_METHOD(SyncReadFileRALongB)
		//{
		//	FileSystem::TFileData data = FileSystem::SyncReadFile("G:/development/game05/build/character_00/application_character_00/x64/Debug/output/shader/static_character_00/screen_quad_vertex_012345678901234567890123456789012345678901234567890.cso");
		//	Assert::IsNotNull(data.get());
		//	Assert::IsTrue(0 != data->size());
		//	Log::RunningUintTest();
		//}

		//TEST_METHOD(SyncReadFileA)
		//{
		//	FileSystem::TFileData data = FileSystem::SyncReadFile("G:/development/game05/build/character_00/application_character_00/x64/Debug/output/shader/static_character_00/screen_quad_vertex.cso");
		//	Assert::IsNotNull(data.get());
		//	Assert::IsTrue(0 != data->size());
		//	Log::RunningUintTest();
		//}

	};
}
