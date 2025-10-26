#include "./BlockManager.h"

#include <unordered_map>
#include <stdexcept>
#include <algorithm>

namespace
{
  using FrequencyTable = std::unordered_map<char32, int32>;

  Optional<char32> NormalizeKanaChar(const char32 ch)
  {
    switch (ch)
    {
    case U'ー':
    case U'－':
    case U'―':
      return none;
    default:
      break;
    }

    static const std::unordered_map<char32, char32> kNormalizationMap = {
      { U'ぁ', U'あ' }, { U'ぃ', U'い' }, { U'ぅ', U'う' }, { U'ぇ', U'え' }, { U'ぉ', U'お' },
      { U'っ', U'つ' }, { U'ゃ', U'や' }, { U'ゅ', U'ゆ' }, { U'ょ', U'よ' }, { U'ゎ', U'わ' },
      { U'ゕ', U'か' }, { U'ゖ', U'け' },
      { U'が', U'か' }, { U'ぎ', U'き' }, { U'ぐ', U'く' }, { U'げ', U'け' }, { U'ご', U'こ' },
      { U'ざ', U'さ' }, { U'じ', U'し' }, { U'ず', U'す' }, { U'ぜ', U'せ' }, { U'ぞ', U'そ' },
      { U'だ', U'た' }, { U'ぢ', U'ち' }, { U'づ', U'つ' }, { U'で', U'て' }, { U'ど', U'と' },
      { U'ば', U'は' }, { U'び', U'ひ' }, { U'ぶ', U'ふ' }, { U'べ', U'へ' }, { U'ぼ', U'ほ' },
      { U'ぱ', U'は' }, { U'ぴ', U'ひ' }, { U'ぷ', U'ふ' }, { U'ぺ', U'へ' }, { U'ぽ', U'ほ' },
      { U'ゔ', U'う' }
    };

    if (const auto it = kNormalizationMap.find(ch); it != kNormalizationMap.end())
    {
      return it->second;
    }

    return ch;
  }

  Array<char32> BuildNormalizedSequence(const Array<String>& source)
  {
    Array<char32> seq;
    seq.reserve(source.size());

    for (const auto& token : source)
    {
      for (const char32 ch : token)
      {
        if (const auto normalized = NormalizeKanaChar(ch))
        {
          seq << *normalized;
        }
      }
    }

    return seq;
  }

  Array<char32> BuildNormalizedSequence(const String& word)
  {
    Array<char32> seq;
    seq.reserve(word.size());

    for (const char32 ch : word)
    {
      if (const auto normalized = NormalizeKanaChar(ch))
      {
        seq << *normalized;
      }
    }

    return seq;
  }

  bool ContainsContiguousSubsequence(const Array<char32>& haystack, const Array<char32>& needle)
  {
    if (needle.isEmpty() || needle.size() > haystack.size())
    {
      return false;
    }

    const size_t H = haystack.size();
    const size_t N = needle.size();

    for (size_t i = 0; i + N <= H; ++i)
    {
      bool ok = true;
      for (size_t j = 0; j < N; ++j)
      {
        if (haystack[i + j] != needle[j])
        {
          ok = false;
          break;
        }
      }
      if (ok) return true;
    }
    return false;
  }

  String RandomHiragana()
  {
    static const Array<char32> hira = {
      U'あ',U'い',U'う',U'え',U'お',
      U'か',U'き',U'く',U'け',U'こ',
      U'さ',U'し',U'す',U'せ',U'そ',
      U'た',U'ち',U'つ',U'て',U'と',
      U'な',U'に',U'ぬ',U'ね',U'の',
      U'は',U'ひ',U'ふ',U'へ',U'ほ',
      U'ま',U'み',U'む',U'め',U'も',
      U'や',U'ゆ',U'よ',
      U'ら',U'り',U'る',U'れ',U'ろ',
      U'わ',U'を',U'ん'
    };
    const size_t idx = static_cast<size_t>(Random(0, static_cast<int32>(hira.size() - 1)));
    return String(1, hira[idx]);
  }

  struct Vec2i { int32 y; int32 x; };

  bool InBounds(int32 y, int32 x, int32 height, int32 width)
  {
    return (0 <= y && y < height && 0 <= x && x < width);
  }

  Array<Vec2i> NextSteps()
  {
    Array<Vec2i> dirs = { Vec2i{+1,0}, Vec2i{0,-1}, Vec2i{0,+1} };
    dirs.shuffle();
    return dirs;
  }
}

BlockManager::BlockManager() = default;
BlockManager::~BlockManager() = default;

Array<String> BlockManager::GetHitWords(const Array<String>& blocks, const Array<String>& dictionary) const
{
  Array<String> result;
  result.reserve(dictionary.size());

  const Array<char32> normalizedBlocks = BuildNormalizedSequence(blocks);

  for (const auto& word : dictionary)
  {
    const Array<char32> normalizedWord = BuildNormalizedSequence(word);
    if (ContainsContiguousSubsequence(normalizedBlocks, normalizedWord))
    {
      result << word;
    }
  }

  return result;
}

Array<std::pair<String, String>> BlockManager::GetReachWords(const Array<String>& /*blocks*/, const Array<String>& /*dictionary*/) const
{
  return {};
}

Array<Array<String>> BlockManager::GenerateBlockGrid(const int32 row, const int32 column, const int32 requestedCount, const int32 attemptsPerWord, const Array<String>& dictionary, const int32 prefetching) const
{
  if (row <= 0 || column <= 0 || requestedCount < 0 || attemptsPerWord <= 0 || dictionary.isEmpty() || prefetching <= 0)
  {
    return {};
  }

  const int32 totalColumns = column * prefetching;
  Array<Array<String>> grid(row, Array<String>(totalColumns, U""));

  // 追加: 実際に配置できた単語の一覧（重複あり、最後に重複排除して出力）
  Array<String> placedWords;
  placedWords.reserve(requestedCount * prefetching);

  // セグメントごとに配置
  for (int32 seg = 0; seg < prefetching; ++seg)
  {
    const int32 xOffset = seg * column;

    int32 placedCount = 0;

    for (int32 trial = 0; trial < requestedCount; ++trial)
    {
      bool placed = false;

      for (int32 attempt = 0; attempt < attemptsPerWord && !placed; ++attempt)
      {
        const String& word = dictionary[Random(0, static_cast<int32>(dictionary.size() - 1))];

        Array<Vec2i> empties;
        for (int32 y = 0; y < row; ++y)
        {
          for (int32 x = 0; x < column; ++x)
          {
            if (grid[y][xOffset + x].isEmpty())
            {
              empties << Vec2i{ y, xOffset + x };
            }
          }
        }

        if (empties.isEmpty())
        {
          break;
        }

        const Vec2i start = empties[Random(0, static_cast<int32>(empties.size() - 1))];

        // 1 文字ずつ配置（曲がり可、上は禁止）
        Vec2i current = start;
        Array<Vec2i> used;
        used.reserve(word.size());

        bool failed = false;

        size_t stepIndex = 0;
        for (const char32 ch : word)
        {
          ++stepIndex;
          const auto normalized = NormalizeKanaChar(ch);
          if (!normalized)
          {
            continue; // 長音はスキップ
          }

          if (!grid[current.y][current.x].isEmpty())
          {
            failed = true;
            break;
          }

          grid[current.y][current.x] = String(1, *normalized);
          used << current;

          // 次の進行先を決める（下・左右、セグメント内、未使用マス）
          bool advanced = false;
          if (stepIndex < word.size())
          {
            for (const auto dir : NextSteps())
            {
              const int32 ny = current.y + dir.y;
              const int32 nx = current.x + dir.x;
              if (InBounds(ny, nx, row, totalColumns)
                && nx >= xOffset && nx < xOffset + column
                && grid[ny][nx].isEmpty())
              {
                current = { ny, nx };
                advanced = true;
                break;
              }
            }

            if (!advanced)
            {
              failed = true;
              break;
            }
          }
        }

        if (failed)
        {
          for (const auto& pos : used)
          {
            grid[pos.y][pos.x].clear();
          }
        }
        else
        {
          placed = true;
          ++placedCount;
          placedWords << word; // 出力用に記録（原文）
        }
      }

      if (placedCount >= requestedCount)
      {
        break;
      }
    }
  }

  // 未充填マスはランダムひらがな
  for (int32 y = 0; y < row; ++y)
  {
    for (int32 x = 0; x < totalColumns; ++x)
    {
      if (grid[y][x].isEmpty())
      {
        grid[y][x] = RandomHiragana();
      }
    }
  }

  // 実際に配置できた単語を Console に出力（重複除去）
  if (!placedWords.isEmpty())
  {
    placedWords.sort();
    // 連続重複除去
    auto newEnd = std::unique(placedWords.begin(), placedWords.end());
    placedWords.erase(newEnd, placedWords.end());

    Console.open();
    Console.writeln(U"[GenerateBlockGrid] Placed words ({}):"_fmt(placedWords.size()));
    for (const auto& w : placedWords)
    {
      Console.writeln(U" - {}"_fmt(w));
    }
  }

  return grid;
}
