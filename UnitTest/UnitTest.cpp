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

    TEST_METHOD(GetHitWords_FindsMatchingWords)
    {
      BlockManager manager;
      Array<String> blocks = { U"す",  U"つ", U"ふ",U"こ" };
      Array<String> dictionary = { U"すこっぷ", U"すこーぷ", U"すもう" };

      const auto result = manager.GetHitWords(blocks, dictionary);

      Assert::AreEqual(static_cast<size_t>(2), result.size());
      Assert::IsTrue(result[0] == U"すこっぷ");
      Assert::IsTrue(result[1] == U"すこーぷ");
    }

    TEST_METHOD(GetHitWords_TreatsVoicedAndSmallKanaAsEquivalent)
    {
      BlockManager manager;
      Array<String> blocks = { U"か", U"な" };
      Array<String> dictionary = { U"がな", U"かな" };

      const auto result = manager.GetHitWords(blocks, dictionary);

      Assert::AreEqual(static_cast<size_t>(2), result.size());
      Assert::IsTrue(result[0] == U"がな");
      Assert::IsTrue(result[1] == U"かな");
    }

    TEST_METHOD(GetReachWords_FindsSingleMissingKana)
    {
      BlockManager manager;
      Array<String> blocks = { U"す", U"こ", U"つ" };
      Array<String> dictionary = { U"すこっぷ", U"すもう" };

      const auto result = manager.GetReachWords(blocks, dictionary);

      Assert::AreEqual(static_cast<size_t>(1), result.size());
      Assert::IsTrue(result[0].first == U"すこっぷ");
      Assert::IsTrue(result[0].second == U"ぷ");
    }

    TEST_METHOD(GetReachWords_ReturnsOriginalCharacterForm)
    {
      BlockManager manager;
      Array<String> blocks = { U"く" };
      Array<String> dictionary = { U"がく" };

      const auto result = manager.GetReachWords(blocks, dictionary);

      Assert::AreEqual(static_cast<size_t>(1), result.size());
      Assert::IsTrue(result[0].first == U"がく");
      Assert::IsTrue(result[0].second == U"が");
    }
  };
}
