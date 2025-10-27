#include "./BlockManager.h"

#include <unordered_map>
#include <stdexcept>
#include <algorithm>

namespace
{
  using FrequencyTable = std::unordered_map<char32, int32>;

  Optional<char32> NormalizeKanaChar(const char32 ch)
  {
    switch (ch) {
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

    if (const auto it = kNormalizationMap.find(ch); it != kNormalizationMap.end()) {
      return it->second;
    }

    return ch;
  }

  Array<char32> BuildNormalizedSequence(const Array<String>& source)
  {
    Array<char32> seq;
    seq.reserve(source.size());

    for (const auto& token : source) {
      for (const char32 ch : token) {
        if (const auto normalized = NormalizeKanaChar(ch)) {
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

    for (const char32 ch : word) {
      if (const auto normalized = NormalizeKanaChar(ch)) {
        seq << *normalized;
      }
    }

    return seq;
  }

  bool ContainsContiguousSubsequence(const Array<char32>& haystack, const Array<char32>& needle)
  {
    if (needle.isEmpty() || needle.size() > haystack.size()) {
      return false;
    }

    const size_t H = haystack.size();
    const size_t N = needle.size();

    for (size_t i = 0; i + N <= H; ++i) {
      bool ok = true;
      for (size_t j = 0; j < N; ++j) {
        if (haystack[i + j] != needle[j]) {
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

  for (const auto& word : dictionary) {
    const Array<char32> normalizedWord = BuildNormalizedSequence(word);
    if (ContainsContiguousSubsequence(normalizedBlocks, normalizedWord)) {
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
  if (row <= 0 || column <= 0 || requestedCount <= 0 || attemptsPerWord <= 0 || dictionary.isEmpty() || prefetching <= 0) {
    return {};
  }

  // row は 1 セグメントあたりの行数として解釈し、総行数は row * prefetching
  const int32 totalRows = row * prefetching;
  const int32 totalColumns = column; // 列はセグメント幅のまま

  Array<Array<String>> grid(totalRows, Array<String>(totalColumns, U""));

  // セグメントごとの配置結果を保持（縦方向に積む）
  Array<Array<String>> segmentPlacedWords;
  segmentPlacedWords.resize(prefetching);

  // セグメントごとに TS 実装に準拠した配置ロジックを適用
  for (int32 seg = 0; seg < prefetching; ++seg) {
    const int32 yOffset = seg * row;

    // clampedCount: min(words, 空きマス) に 1..requestedCount でクランプ
    const int32 upperBound = static_cast<int32>(Min(static_cast<size_t>(dictionary.size()), static_cast<size_t>(row * column)));
    int32 remainingQuota = Max<int32>(1, Min<int32>(requestedCount, upperBound));
    const int32 attemptLimit = Max<int32>(1, attemptsPerWord);

    // 候補は一度 shuffle し、pop で一意に取り出す
    Array<String> shuffledWords = dictionary;
    shuffledWords.shuffle();

    auto getEmptyCells = [&](Array<Vec2i>& out)
      {
        out.clear();
        out.reserve(static_cast<size_t>(row * column));
        for (int32 y = 0; y < row; ++y) {
          for (int32 x = 0; x < column; ++x) {
            const int32 gy = yOffset + y;
            if (grid[gy][x].isEmpty()) {
              out << Vec2i{ gy, x };
            }
          }
        }
      };

    auto placeWordAlongPath = [&](const String& word) -> bool
      {
        if (word.isEmpty()) return false;

        // 文字配列（正規化せず、そのまま配置）
        Array<char32> letters;
        letters.reserve(word.size());
        for (const char32 ch : word) {
          letters << ch;
        }

        Array<Vec2i> empties;
        getEmptyCells(empties);

        if (letters.size() > empties.size() || empties.isEmpty()) {
          return false;
        }

        const Vec2i start = empties[static_cast<size_t>(Random(0, static_cast<int32>(empties.size() - 1)))];
        Array<Vec2i> placements;
        placements << start;
        grid[start.y][start.x] = String(1, letters[0]);

        for (size_t i = 1; i < letters.size(); ++i) {
          const Vec2i prev = placements.back();

          // 近傍（下・左右）をランダム順に走査し、最初の空きに置く
          bool moved = false;
          for (const auto dir : NextSteps()) {
            const int32 ny = prev.y + dir.y;
            const int32 nx = prev.x + dir.x;
            if (InBounds(ny, nx, totalRows, totalColumns)
              && ny >= yOffset && ny < yOffset + row
              && grid[ny][nx].isEmpty()) {
              grid[ny][nx] = String(1, letters[i]);
              placements << Vec2i{ ny, nx };
              moved = true;
              break;
            }
          }

          if (!moved) {
            // ロールバック
            for (const auto& cell : placements) {
              grid[cell.y][cell.x].clear();
            }
            return false;
          }
        }

        return true;
      };

    while (remainingQuota > 0 && !shuffledWords.isEmpty()) {
      // セグメント内の空きが無ければ終了
      Array<Vec2i> empties;
      getEmptyCells(empties);
      if (empties.isEmpty()) {
        break;
      }

      const String candidate = shuffledWords.back();
      shuffledWords.pop_back();

      bool placed = false;
      for (int32 attempt = 0; attempt < attemptLimit; ++attempt) {
        if (placeWordAlongPath(candidate)) {
          placed = true;
          segmentPlacedWords[seg] << candidate;
          break;
        }
      }

      // 成功/失敗に関わらずクオータを減算
      --remainingQuota;
    }
  }

  // 未充填マスはランダムひらがな
  for (int32 y = 0; y < totalRows; ++y) {
    for (int32 x = 0; x < totalColumns; ++x) {
      if (grid[y][x].isEmpty()) {
        grid[y][x] = U"*";
      }
    }
  }

  // 実際に配置できた単語を Console に出力（セグメント境界を明示）
  bool anyPlaced = false;
  for (const auto& v : segmentPlacedWords) {
    if (!v.isEmpty()) { anyPlaced = true; break; }
  }

  if (anyPlaced) {
    Console.open();
    Console.writeln(U"[GenerateBlockGrid] Prefetch segments: {}"_fmt(prefetching));

    for (int32 seg = 0; seg < prefetching; ++seg) {
      const int32 yOffset = seg * row;
      const int32 fromRow = yOffset;
      const int32 toRow = yOffset + row - 1;

      Array<String> words = segmentPlacedWords[seg];
      words.sort();
      auto newEnd = std::unique(words.begin(), words.end());
      words.erase(newEnd, words.end());

      Console.writeln(U"--- Segment {}/{} [rows {}..{}] : {} word(s) ---"_fmt(seg + 1, prefetching, fromRow, toRow, words.size()));
      for (const auto& w : words) {
        Console.writeln(U"  - {}"_fmt(w));
      }
    }
  }

  return grid;
}
