#include "./BlockManager.h"

#include <unordered_map>
#include <stdexcept>

namespace
{
  using FrequencyTable = std::unordered_map<char32, int32>;

  /// <summary>
  /// ひらがな1文字をゲーム内ルールに沿って正規化する。
  /// ・濁点／半濁点付き文字は清音へ集約
  /// ・小書き文字は通常サイズへ置換
  /// ・長音記号は完全に無視（= none を返す）
  /// </summary>
  /// <param name="ch">入力された1文字</param>
  /// <returns>正規化後の文字。長音記号の場合は none。</returns>
  Optional<char32> NormalizeKanaChar(const char32 ch)
  {
    switch (ch)
    {
    case U'ー': // 一般的な長音符号
    case U'－': // 全角ハイフン（長音として扱う）
    case U'―': // ダッシュ（長音扱い）
      return none;
    default:
      break;
    }

    static const std::unordered_map<char32, char32> kNormalizationMap = {
      // 小書き文字 -> 通常字
      { U'ぁ', U'あ' }, { U'ぃ', U'い' }, { U'ぅ', U'う' }, { U'ぇ', U'え' }, { U'ぉ', U'お' },
      { U'っ', U'つ' }, { U'ゃ', U'や' }, { U'ゅ', U'ゆ' }, { U'ょ', U'よ' }, { U'ゎ', U'わ' },
      { U'ゕ', U'か' }, { U'ゖ', U'け' },

      // 濁点・半濁点付き文字 -> 清音
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

  /// <summary>
  /// ブロック配列（または文字列）から、正規化済みの線形シーケンスを生成する
  /// （長音は除去、小書き・濁点は統合）。
  /// </summary>
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

  /// <summary>
  /// haystack に needle の連続した並びが含まれるか（部分列/部分文字列判定）。
  /// </summary>
  bool ContainsContiguousSubsequence(const Array<char32>& haystack, const Array<char32>& needle)
  {
    if (needle.isEmpty())
    {
      return false;
    }

    if (needle.size() > haystack.size())
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

      if (ok)
      {
        return true;
      }
    }

    return false;
  }

  /// 既存の頻度表ベースのヘルパ（現状は GenerateBlockGrid のみで使用）
  struct Dummy { };
  FrequencyTable BuildFrequency(const Array<String>& source)
  {
    FrequencyTable table;

    for (const auto& token : source)
    {
      for (const char32 ch : token)
      {
        if (const auto normalized = NormalizeKanaChar(ch))
        {
          ++table[*normalized];
        }
      }
    }

    return table;
  }

  FrequencyTable BuildFrequency(const String& word)
  {
    FrequencyTable table;

    for (const char32 ch : word)
    {
      if (const auto normalized = NormalizeKanaChar(ch))
      {
        ++table[*normalized];
      }
    }

    return table;
  }

} // namespace

BlockManager::BlockManager() = default;
BlockManager::~BlockManager() = default;

Array<String> BlockManager::GetHitWords(const Array<String>& blocks, const Array<String>& dictionary) const
{
  Array<String> result;
  result.reserve(dictionary.size());

  // 新ルール: 並び順も考慮し、A（ブロック列）内に B（単語）の正規化列が
  // 連続して現れるか（部分文字列）で判定する。
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
  // いったんリーチ判定は未対応（空配列を返却）
  return {};
}

Array<Array<String>> BlockManager::GenerateBlockGrid(const int32 row, const int32 column, const int32 batchSize, const Array<String>& dictionary) const
{
  Array<Array<String>> grid;

  if (row <= 0 || column <= 0 || batchSize <= 0 || dictionary.isEmpty())
  {
    return grid;
  }

  const int32 requiredSize = row * column;
  if (requiredSize % batchSize != 0)
  {
    throw std::invalid_argument("row * column must be a multiple of batchSize.");
  }

  Array<String> candidateChars;
  candidateChars.reserve(requiredSize);

  Array<String> shuffledWords = dictionary;
  Array<String> wordCandidates;

  while (candidateChars.size() < requiredSize)
  {
    shuffledWords.shuffle();
    wordCandidates.clear();

    int32 accumulated = 0;

    for (const auto& word : shuffledWords)
    {
      wordCandidates << word;
      accumulated += static_cast<int32>(word.size());

      if (accumulated >= batchSize)
      {
        break;
      }
    }

    if (wordCandidates.isEmpty())
    {
      break;
    }

    wordCandidates.shuffle();

    Array<String> charBatch;
    charBatch.reserve(accumulated);

    for (const auto& word : wordCandidates)
    {
      for (const char32 ch : word)
      {
        if (const auto normalized = NormalizeKanaChar(ch))
        {
          charBatch << String(1, *normalized);
        }
      }
    }

    charBatch.shuffle();

    for (const auto& ch : charBatch)
    {
      candidateChars << ch;

      if (candidateChars.size() >= requiredSize)
      {
        break;
      }
    }

    if (candidateChars.size() >= requiredSize)
    {
      break;
    }
  }

  grid.reserve(row);
  size_t index = 0;

  for (int32 r = 0; r < row; ++r)
  {
    Array<String> line;
    line.reserve(column);

    for (int32 c = 0; c < column; ++c)
    {
      if (index < candidateChars.size())
      {
        line << candidateChars[index++];
      }
      else
      {
        line << String();
      }
    }

    grid << std::move(line);
  }

  return grid;
}
