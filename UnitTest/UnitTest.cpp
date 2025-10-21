#include "pch.h"
#include "CppUnitTest.h"
#include "../Ich/System/System/BlockManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Forward declaration for the function implemented in Sample.cpp.
int Add(int left, int right);
extern Array<String> keywords;

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

    TEST_METHOD(GetHitWords_IntegratesWithKeywordDictionary)
    {
      BlockManager manager;
      const Array<String> blocks = { U"か", U"わ", U"る", U"め", U"を" };

      const auto result = manager.GetHitWords(blocks, keywords);

      Assert::AreEqual(static_cast<size_t>(1), result.size());
      Assert::IsTrue(result.contains(U"わかめ"));
      Assert::IsFalse(result.contains(U"わかれる"));
    }

    TEST_METHOD(GetReachWords_IntegratesWithKeywordDictionary)
    {
      BlockManager manager;
      const Array<String> blocks = { U"ら", U"わ" };

      const auto result = manager.GetReachWords(blocks, keywords);

      Assert::AreEqual(static_cast<size_t>(2), result.size());
      Assert::IsTrue(result[0].first == U"かわら");
      Assert::IsTrue(result[0].second == U"か");
      Assert::IsTrue(result[1].first == U"わらう");
      Assert::IsTrue(result[1].second == U"う");
    }

    TEST_METHOD(GenerateBlockGrid_ReturnsGridWithRequestedSize)
    {
      BlockManager manager;
      const Array<String> dictionary = { U"わかめ", U"わかれる" };

      const auto grid = manager.GenerateBlockGrid(2, 3, 5, dictionary);

      Assert::AreEqual(static_cast<size_t>(2), grid.size());

      for (const auto& row : grid)
      {
        Assert::AreEqual(static_cast<size_t>(3), row.size());

        for (const auto& cell : row)
        {
          Assert::IsTrue(cell.size() <= 1);
        }
      }
    }
  };
}
