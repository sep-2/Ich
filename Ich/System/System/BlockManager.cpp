#include "./BlockManager.h"

#include <unordered_map>

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

  int32 GetAvailableCount(const FrequencyTable& table, const char32 key)
  {
    if (const auto it = table.find(key); it != table.end())
    {
      return it->second;
    }

    return 0;
  }

  String DetermineMissingCharacter(const String& word, const char32 missingNormalized, FrequencyTable available)
  {
    for (const char32 ch : word)
    {
      if (const auto normalized = NormalizeKanaChar(ch))
      {
        if (const auto it = available.find(*normalized); it != available.end() && it->second > 0)
        {
          --(it->second);
        }
        else if (*normalized == missingNormalized)
        {
          return String(1, ch);
        }
      }
    }

    return String(1, missingNormalized);
  }
}

BlockManager::BlockManager() = default;
BlockManager::~BlockManager() = default;

Array<String> BlockManager::GetHitWords(const Array<String>& blocks, const Array<String>& dictionary) const
{
  Array<String> result;
  result.reserve(dictionary.size());

  const FrequencyTable blockFrequency = BuildFrequency(blocks);

  for (const auto& word : dictionary)
  {
    const FrequencyTable wordFrequency = BuildFrequency(word);

    bool canBuild = true;

    for (const auto& [kana, required] : wordFrequency)
    {
      if (GetAvailableCount(blockFrequency, kana) < required)
      {
        canBuild = false;
        break;
      }
    }

    if (canBuild)
    {
      result << word;
    }
  }

  return result;
}

Array<std::pair<String, String>> BlockManager::GetReachWords(const Array<String>& blocks, const Array<String>& dictionary) const
{
  Array<std::pair<String, String>> result;
  result.reserve(dictionary.size());

  const FrequencyTable blockFrequency = BuildFrequency(blocks);

  for (const auto& word : dictionary)
  {
    const FrequencyTable wordFrequency = BuildFrequency(word);

    int32 deficitCount = 0;
    char32 missingKana = U'\0';

    for (const auto& [kana, required] : wordFrequency)
    {
      const int32 available = GetAvailableCount(blockFrequency, kana);
      if (available < required)
      {
        deficitCount += (required - available);
        missingKana = kana;

        if (deficitCount > 1)
        {
          break;
        }
      }
    }

    if (deficitCount == 1)
    {
      const String missing = DetermineMissingCharacter(word, missingKana, blockFrequency);
      result.emplace_back(word, missing);
    }
  }

  return result;
}
