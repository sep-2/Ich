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
  /// 文字列配列から各文字の出現回数テーブルを構築する。
  /// ブロック（複数文字が含まれる可能性）を一括で処理する際に利用。
  /// </summary>
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

  /// <summary>
  /// 単語（1要素）から必要文字数を算出するヘルパー。
  /// ヒット／リーチ判定では毎回辞書語をこのテーブルに変換して利用する。
  /// </summary>
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

  /// <summary>
  /// 指定した文字の保有数を安全に取得する。
  /// unordered_map::operator[] は意図せずエントリを作ってしまうため find を利用する。
  /// </summary>
  int32 GetAvailableCount(const FrequencyTable& table, const char32 key)
  {
    if (const auto it = table.find(key); it != table.end())
    {
      return it->second;
    }

    return 0;
  }

  /// <summary>
  /// リーチ判定で不足している「元の文字（濁点付き等）」を特定する。
  /// 正規化済みテーブルでは区別が付かないため、単語を頭から走査し
  /// 利用可能数を減算しながら最初に不足する実文字を返す。
  /// </summary>
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

    // 上記ループで返却できなかった場合は、正規化後の文字をそのまま返す。
    // （辞書に想定外の表記が含まれていたケースのフォールバック）
    return String(1, missingNormalized);
  }
} // namespace

BlockManager::BlockManager() = default;
BlockManager::~BlockManager() = default;

Array<String> BlockManager::GetHitWords(const Array<String>& blocks, const Array<String>& dictionary) const
{
  Array<String> result;
  result.reserve(dictionary.size());

  // blocksを連結して1つの文字列にする
  String concatenated;
  for (const auto& block : blocks)
  {
    concatenated += block;
  }

  // 各辞書語について、連結文字列内に順番通りに含まれているかチェック
  for (const auto& word : dictionary)
  {
    // 辞書語が空の場合はスキップ
    if (word.isEmpty())
    {
      continue;
    }

    // 連結文字列内で辞書語を検索
    size_t pos = concatenated.indexOf(word);
    
    // 見つかった場合は結果に追加
    if (pos != String::npos)
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

  // ブロックの保有数はヒット判定と同様に事前計算しておく。
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

        // リーチは「足りない文字が1つだけ」のケースのため、
        // 不足数が2以上になった時点で判定終了。
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

Array<Array<String>> BlockManager::GenerateBlockGrid(const int32 row, const int32 column, const int32 batchSize, const Array<String>& dictionary) const
{
  Array<Array<String>> grid;

  // 生成条件が満たされない場合は空配列を返却する（早期リターン）。
  if (row <= 0 || column <= 0 || batchSize <= 0 || dictionary.isEmpty())
  {
    return grid;
  }

  const int32 requiredSize = row * column;
  if (requiredSize % batchSize != 0)
  {
    throw std::invalid_argument("row * column must be a multiple of batchSize.");
  }

  // 抽出した文字を一次元で蓄えるバッファ。後で二次元配列へ整形する。
  Array<String> candidateChars;
  candidateChars.reserve(requiredSize);

  // 辞書語を都度シャッフルして利用するためのバッファと、一度のループで使用する語リスト。
  Array<String> shuffledWords = dictionary;
  Array<String> wordCandidates;

  // 必要な文字数を満たすまで辞書語をシャッフルしながら候補文字を追加していく。
  while (candidateChars.size() < requiredSize)
  {
    shuffledWords.shuffle();
    wordCandidates.clear();

    int32 accumulated = 0;

    // batchSize に到達するまで辞書語を積み上げて候補リストに加える。
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
      // 辞書が空、または有効な語を取得できない場合は処理を終了する。
      break;
    }

    // 候補語自体をシャッフルし、元となる単語の順序を均一化する。
    wordCandidates.shuffle();

    // 各候補語の文字を 1 文字ずつ取り出して候補文字リストに格納。
    for (const auto& word : wordCandidates)
    {
      for (const char32 ch : word)
      {
        candidateChars << String(1, ch);

        if (candidateChars.size() >= requiredSize)
        {
          // 必要数を満たしたら即座に外側のループへ抜ける。
          break;
        }
      }

      if (candidateChars.size() >= requiredSize)
      {
        break;
      }
    }
  }

  // 候補文字リストを行列構造に再配置する。
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
        // 候補が不足した場合は空文字を詰めてサイズを合わせる。
        line << String();
      }
    }

    grid << std::move(line);
  }

  return grid;
}
