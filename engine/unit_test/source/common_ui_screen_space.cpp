#include "unit_test_pch.h"
#include "common/ui/ui_screen_space.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

/*
geometry
	  ______ 1,1
	  |	 |
	  |	 |
-1,-1 |_____|

uv
	______ 1,0
	|	 |
	|	 |
0,1 |_____|

*/

namespace CommonUIScreenSpace
{
	TEST_CLASS(CheckCalculations)
	{
		public:
		TEST_METHOD(Simple)
		{
			const UIScreenSpace parent;
			UIScreenSpace child;
			child.Update(
				parent, 
				VectorFloat4(-0.5f, -0.5f, 0.5f, 0.5f), 
				VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f)
				);
			
			const auto pos = child.GetPosRef();
			Assert::AreEqual(pos[0], -0.5f, 0.0000001f);
			Assert::AreEqual(pos[1], -0.5f, 0.0000001f);
			Assert::AreEqual(pos[2], 0.5f, 0.0000001f);
			Assert::AreEqual(pos[3], 0.5f, 0.0000001f);

			const auto clip = child.GetClipRef();
			Assert::AreEqual(clip[0], -0.5f, 0.0000001f);
			Assert::AreEqual(clip[1], -0.5f, 0.0000001f);
			Assert::AreEqual(clip[2], 0.5f, 0.0000001f);
			Assert::AreEqual(clip[3], 0.5f, 0.0000001f);
		}

		TEST_METHOD(UV_A)
		{
			const UIScreenSpace parent;
			UIScreenSpace child;
			child.Update(
				parent, 
				VectorFloat4(-0.5f, -0.5f, 0.5f, 0.5f), 
				VectorFloat4(0.0f, 0.5f, 1.0f, 0.0f)
				);

			{
				const auto pos = child.GetPosRef();
				Assert::AreEqual(pos[0], -0.5f, 0.0000001f);
				Assert::AreEqual(pos[1], -1.5f, 0.0000001f);
				Assert::AreEqual(pos[2], 0.5f, 0.0000001f);
				Assert::AreEqual(pos[3], 0.5f, 0.0000001f);

				const auto clip = child.GetClipRef();
				Assert::AreEqual(clip[0], -0.5f, 0.0000001f);
				Assert::AreEqual(clip[1], -0.5f, 0.0000001f);
				Assert::AreEqual(clip[2], 0.5f, 0.0000001f);
				Assert::AreEqual(clip[3], 0.5f, 0.0000001f);
			}

			UIScreenSpace child_child;
			child_child.Update(
				child,
				VectorFloat4(-1.0f, 0.0f, 1.0f, 1.0f), 
				VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f)
				);
			{
				const auto pos = child_child.GetPosRef();
				Assert::AreEqual(pos[0], -0.5f, 0.0000001f);
				Assert::AreEqual(pos[1], -0.5f, 0.0000001f);
				Assert::AreEqual(pos[2], 0.5f, 0.0000001f);
				Assert::AreEqual(pos[3], 0.5f, 0.0000001f);

				const auto clip = child_child.GetClipRef();
				Assert::AreEqual(clip[0], -0.5f, 0.0000001f);
				Assert::AreEqual(clip[1], -0.5f, 0.0000001f);
				Assert::AreEqual(clip[2], 0.5f, 0.0000001f);
				Assert::AreEqual(clip[3], 0.5f, 0.0000001f);
			}

		}

		TEST_METHOD(UV_B)
		{
			const UIScreenSpace parent;
			UIScreenSpace child;
			child.Update(
				parent, 
				VectorFloat4(-0.5f, -0.5f, 0.5f, 0.5f), 
				VectorFloat4(0.0f, 1.0f, 1.0f, 0.5f)
				);

			{
				const auto pos = child.GetPosRef();
				Assert::AreEqual(pos[0], -0.5f, 0.0000001f);
				Assert::AreEqual(pos[1], -0.5f, 0.0000001f);
				Assert::AreEqual(pos[2], 0.5f, 0.0000001f);
				Assert::AreEqual(pos[3], 1.5f, 0.0000001f);

				const auto clip = child.GetClipRef();
				Assert::AreEqual(clip[0], -0.5f, 0.0000001f);
				Assert::AreEqual(clip[1], -0.5f, 0.0000001f);
				Assert::AreEqual(clip[2], 0.5f, 0.0000001f);
				Assert::AreEqual(clip[3], 0.5f, 0.0000001f);
			}


			UIScreenSpace child_child;
			child_child.Update(
				child,
				VectorFloat4(-1.0f, 0.0f, 1.0f, 1.0f), 
				VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f)
				);

			{
				const auto pos = child_child.GetPosRef();
				Assert::AreEqual(pos[0], -0.5f, 0.0000001f);
				Assert::AreEqual(pos[1], 0.5f, 0.0000001f);
				Assert::AreEqual(pos[2], 0.5f, 0.0000001f);
				Assert::AreEqual(pos[3], 1.5f, 0.0000001f);

				const auto clip = child_child.GetClipRef();
				Assert::AreEqual(clip[0], -0.5f, 0.0000001f);
				Assert::AreEqual(clip[1], 0.5f, 0.0000001f);
				Assert::AreEqual(clip[2], 0.5f, 0.0000001f);
				Assert::AreEqual(clip[3], 0.5f, 0.0000001f);
			}

		}

	};
}
