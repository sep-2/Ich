#include "pch.h"
#include "CppUnitTest.h"
#include "../Ich/System/System/BlockManager.h"
#include "../Ich/Keywords.hpp"
#include <algorithm>
#include <utility>
#include <stdexcept>

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
      // 並び順を考慮。"すこーぷ" が A 内に連続で現れるかを検証
      Array<String> blocks = { U"す", U"こ", U"ふ" };
      Array<String> dictionary = { U"すこっぷ", U"すこーぷ", U"すもう" };

      const auto result = manager.GetHitWords(blocks, dictionary);

      Assert::AreEqual(static_cast<size_t>(1), result.size());
      Assert::IsTrue(result[0] == U"すこーぷ");
    }

    TEST_METHOD(GetHitWords_TreatsVoicedAndSmallKanaAsEquivalent)
    {
      BlockManager manager;
      // 並び順考慮で "かな" / "がな" が一致（濁点同一視）
      Array<String> blocks = { U"か", U"な" };
      Array<String> dictionary = { U"がな", U"かな" };

      const auto result = manager.GetHitWords(blocks, dictionary);

      Assert::AreEqual(static_cast<size_t>(2), result.size());
      Assert::IsTrue(result.contains(U"がな"));
      Assert::IsTrue(result.contains(U"かな"));
    }

    TEST_METHOD(GetHitWords_IntegratesWithKeywordDictionary)
    {
      BlockManager manager;
      // 並び順を満たすようにブロックを並べる
      const Array<String> blocks = { U"わ", U"か", U"め" };

      const auto result = manager.GetHitWords(blocks, keywords);

      Assert::IsTrue(result.contains(U"わかめ"));
      // 並び順の都合で "わかれる" は含まれない
      Assert::IsFalse(result.contains(U"わかれる"));
    }

    TEST_METHOD(GenerateBlockGrid_ReturnsGridWithRequestedSize)
    {
      BlockManager manager;
      const Array<String> dictionary = { U"わかめ", U"わかれる" };

      const int32 row = 2;
      const int32 column = 3;
      const int32 requestedCount = 3;
      const int32 attemptsPerWord = 5;
      const int32 prefetching = 1;

      const auto grid = manager.GenerateBlockGrid(row, column, requestedCount, attemptsPerWord, dictionary, prefetching);

      Assert::AreEqual(static_cast<size_t>(row), grid.size());

      const auto flattened = FlattenGrid(grid, column * prefetching);
      Assert::AreEqual(static_cast<size_t>(row * column * prefetching), flattened.size());

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
      const int32 requestedCount = 2;
      const int32 attemptsPerWord = 5;

      const auto grid = manager.GenerateBlockGrid(row, column, requestedCount, attemptsPerWord, dictionary);

      Assert::AreEqual(static_cast<size_t>(row), grid.size());

      const auto flattened = FlattenGrid(grid, column);
      Assert::AreEqual(static_cast<size_t>(row * column), flattened.size());

      for (const auto& cell : flattened)
      {
        Assert::IsTrue(cell.size() <= 1);
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

