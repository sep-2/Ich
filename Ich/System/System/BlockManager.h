#pragma once

#include <Siv3D.hpp>
#include <utility>

/// <summary>
/// ひらがなブロックの集合をもとに、辞書内の単語が成立するかどうかを判定するためのユーティリティ。
/// ゲーム中では、手元のブロックと辞書を渡すだけでヒット・リーチの抽出が行えるようにする。
/// </summary>
class BlockManager
{
public:
  BlockManager();
  ~BlockManager();

  /// <summary>
  /// ブロックだけで完全に組み立てられる（=ヒットする）単語を抽出する。
  /// ブロックと辞書内の単語は、濁点・半濁点・小文字を区別せずに突き合わせる。
  /// </summary>
  /// <param name="blocks">現在保持しているブロック一覧。1要素につき1文字を想定。</param>
  /// <param name="dictionary">判定対象となる単語の一覧。</param>
  /// <returns>ヒットした単語を辞書順のまま返す。</returns>
  Array<String> GetHitWords(const Array<String>& blocks, const Array<String>& dictionary) const;

  /// <summary>
  /// ブロックにあと1文字加えるだけで完成する（=リーチ状態の）単語を抽出する。
  /// 足りない文字は正規化後ではなく「辞書に記載されている元の文字」を返却する。
  /// </summary>
  /// <param name="blocks">現在保持しているブロック一覧。</param>
  /// <param name="dictionary">判定対象の単語一覧。</param>
  /// <returns>
  /// first: 単語そのもの / second: 足りない文字（辞書の表記に合わせた1文字）。
  /// </returns>
  Array<std::pair<String, String>> GetReachWords(const Array<String>& blocks, const Array<String>& dictionary) const;

  /// <summary>
  /// 辞書の単語群からランダムに文字を抜き出し、row × column のブロック配置を生成する。
  /// blockSize 文字分を目安に辞書から文字を確保し、二次元配列に整形して返す。
  /// </summary>
  /// <param name="row">生成する行数。1 以上であること。</param>
  /// <param name="column">生成する列数。1 以上であること。</param>
  /// <param name="blockSize">候補抽出時に確保したい最小文字数。辞書語を順に積み上げて到達させる。</param>
  /// <param name="dictionary">ブロック候補として利用する単語一覧。空の場合は空配列を返す。</param>
  /// <returns>row × column のサイズを持つブロック配列。条件を満たせない場合は空配列。</returns>
  Array<Array<String>> GenerateBlockGrid(int32 row, int32 column, int32 blockSize, const Array<String>& dictionary) const;
};
