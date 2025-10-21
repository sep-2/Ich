#include "pch.h"
#include "CppUnitTest.h"
#include "../Ich/System/System/BlockManager.h"
#include <algorithm>
#include <utility>
#include <stdexcept>

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
      const Array<String> blocks = { U"か", U"わ", U"る", U"め", U"を" };

      const auto result = manager.GetReachWords(blocks, keywords);

      Assert::IsTrue(result.contains(std::make_pair(String(U"わかれる"), String(U"れ"))));
    }

    TEST_METHOD(GenerateBlockGrid_ReturnsGridWithRequestedSize)
    {
      BlockManager manager;
      const Array<String> dictionary = { U"わかめ", U"わかれる" };

      Assert::ExpectException<std::invalid_argument>([&]()
      {
        manager.GenerateBlockGrid(2, 3, 5, dictionary);
      });

      const auto grid = manager.GenerateBlockGrid(2, 3, 3, dictionary);

      Assert::AreEqual(static_cast<size_t>(2), grid.size());

      const auto flattened = FlattenGrid(grid, 3);
      Assert::AreEqual(static_cast<size_t>(2 * 3), flattened.size());

      for (const auto& cell : flattened)
      {
        Assert::IsTrue(cell.size() <= 1);
      }
    }

    TEST_METHOD(GenerateBlockGrid_HandlesDictionarySmallerThanGrid)
    {
      BlockManager manager;
      const Array<String> dictionary = { U"あ", U"い" };

      const int32 row = 2;
      const int32 column = 3;
      const auto grid = manager.GenerateBlockGrid(row, column, 2, dictionary);

      Assert::AreEqual(static_cast<size_t>(row), grid.size());

      const auto flattened = FlattenGrid(grid, column);
      Assert::AreEqual(static_cast<size_t>(row * column), flattened.size());

      for (const auto& cell : flattened)
      {
        Assert::IsTrue(cell == U"あ" || cell == U"い");
      }
    }

    TEST_METHOD(GenerateBlockGrid_FillsWhenGridIsMultipleOfBatchSize)
    {
      BlockManager manager;
      const Array<String> dictionary = { U"かき", U"くけこ" };
      const int32 row = 3;
      const int32 column = 4;
      const int32 batchSize = 3;

      const auto grid = manager.GenerateBlockGrid(row, column, batchSize, dictionary);

      Assert::AreEqual(static_cast<size_t>(row), grid.size());

      const auto flattened = FlattenGrid(grid, column);
      Assert::AreEqual(static_cast<size_t>(row * column), flattened.size());

      for (const auto& cell : flattened)
      {
        Assert::IsTrue(cell == U"か" || cell == U"き" || cell == U"く" || cell == U"け" || cell == U"こ");
      }
    }

    TEST_METHOD(GenerateBlockGrid_FillsWhenGridIsMultipleOfBatchSizePlusOne)
    {
      BlockManager manager;
      const Array<String> dictionary = { U"さしす", U"せそたち" };
      const int32 row = 3;
      const int32 column = 3;
      const int32 batchSize = 4;

      Assert::ExpectException<std::invalid_argument>([&]()
      {
        manager.GenerateBlockGrid(row, column, batchSize, dictionary);
      });
    }

    TEST_METHOD(GenerateBlockGrid_IncludesAllWordsWhenExactSize)
    {
      BlockManager manager;
      const Array<String> dictionary = { U"なに", U"ぬね" };
      const int32 row = 2;
      const int32 column = 2;
      const int32 batchSize = 4;

      const auto grid = manager.GenerateBlockGrid(row, column, batchSize, dictionary);

      Assert::AreEqual(static_cast<size_t>(row), grid.size());

      auto flattened = FlattenGrid(grid, column);

      for (const auto& word : dictionary)
      {
        for (const char32 ch : word)
        {
          const String target(1, ch);
          const auto it = std::find(flattened.begin(), flattened.end(), target);
          Assert::IsTrue(it != flattened.end(), L"必要な文字が生成結果に含まれていません。");
          if (it != flattened.end())
          {
            flattened.erase(it);
          }
        }
      }
    }

  private:
    static Array<String> FlattenGrid(const Array<Array<String>>& grid, const int32 expectedColumn)
    {
      Array<String> flattened;
      for (const auto& line : grid)
      {
        Assert::AreEqual(static_cast<size_t>(expectedColumn), line.size());
        for (const auto& cell : line)
        {
          flattened << cell;
        }
      }
      return flattened;
    }
  };
}

