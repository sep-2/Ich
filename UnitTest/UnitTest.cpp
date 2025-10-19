#include "pch.h"
#include "CppUnitTest.h"

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
}
