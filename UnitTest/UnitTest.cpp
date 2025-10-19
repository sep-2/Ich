#include "pch.h"
#include "CppUnitTest.h"
#include "../Ich/System/System/BlockManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Forward declaration for the function implemented in Sample.cpp.
int Add(int left, int right);

namespace UnitTest
{
	TEST_CLASS(SampleTests)
	{
	public:
		
		TEST_METHOD(Add_ReturnsSumForPositiveValues)
		{
			Assert::AreEqual(5, Add(2, 3));
			Assert::AreEqual(0, Add(0, 0));
		}

		TEST_METHOD(Add_HandlesNegativeValues)
		{
			Assert::AreEqual(-1, Add(2, -3));
			Assert::AreEqual(-5, Add(-2, -3));
		}
	}; 

	TEST_CLASS(BlockManagerTests)
	{
	public:

		TEST_METHOD(GetHitWords_ReturnsEmptyArray)
		{
			BlockManager manager;
			Array<String> blocks = { U"あ", U"い" };
			Array<String> dictionary = { U"あい" };
			const auto result = manager.GetHitWords(blocks, dictionary);
			Assert::IsTrue(result.isEmpty());
		}

		TEST_METHOD(GetReachWords_ReturnsEmptyArray)
		{
			BlockManager manager;
			Array<String> blocks = { U"か" };
			Array<String> dictionary = { U"かきく" };
			const auto result = manager.GetReachWords(blocks, dictionary);
			Assert::IsTrue(result.isEmpty());
		}
	};
}
