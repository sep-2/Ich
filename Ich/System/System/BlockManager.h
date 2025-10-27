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
  /// 新仕様のブロックグリッド生成。
  /// 返り値は (文字, フラグ) の 2 次元配列。フラグは単語の先頭文字なら true。
  /// 指定行列サイズの領域を prefetching 回縦に連結して作成し、
  /// セグメントごとに最大 requestedCount 個の単語を下・左右へ曲がりながら配置する。
  /// 残りはプレースホルダーで埋める。
  /// </summary>
  /// <param name="row">行数</param>
  /// <param name="column">列数（1 セグメントの幅）</param>
  /// <param name="requestedCount">1 セグメントで試みる単語数の上限</param>
  /// <param name="attemptsPerWord">1 単語あたりの試行回数</param>
  /// <param name="dictionary">配置に用いる単語辞書</param>
  /// <param name="segmentCount">セグメント数。デフォルト 1</param>
  Array<Array<std::pair<String, bool>>> GenerateBlockGrid(int32 row, int32 column, int32 requestedCount, int32 attemptsPerWord, const Array<String>& dictionary, int32 segmentCount = 1) const;
};
