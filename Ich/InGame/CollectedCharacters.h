#pragma once
#include <Siv3D.hpp>

/// <summary>
/// 獲得した文字を管理するクラス
/// </summary>
class CollectedCharacters
{
public:
  CollectedCharacters();
  ~CollectedCharacters();

  /// <summary>
  /// 文字を追加
  /// </summary>
  /// <param name="character">追加する文字</param>
  void AddCharacter(const String& character);

  /// <summary>
  /// 文字配列を取得
  /// </summary>
  /// <returns>現在の文字配列</returns>
  [[nodiscard]]
  const Array<String>& GetCharacters() const;

  /// <summary>
  /// 文字配列をクリア
  /// </summary>
  void Clear();

  /// <summary>
  /// 最大文字数を設定
  /// </summary>
  /// <param name="maxSize">最大文字数</param>
  void SetMaxSize(size_t maxSize);

  /// <summary>
  /// 最大文字数を取得
  /// </summary>
  /// <returns>最大文字数</returns>
  [[nodiscard]]
  size_t GetMaxSize() const;

  /// <summary>
  /// 現在の文字数を取得
  /// </summary>
  /// <returns>現在の文字数</returns>
  [[nodiscard]]
  size_t GetSize() const;

  /// <summary>
  /// 文字配列が満杯かどうか
  /// </summary>
  /// <returns>満杯の場合true</returns>
  [[nodiscard]]
  bool IsFull() const;

  /// <summary>
  /// 文字配列が空かどうか
  /// </summary>
  /// <returns>空の場合true</returns>
  [[nodiscard]]
  bool IsEmpty() const;

  /// <summary>
  /// 連結した文字列を取得
  /// </summary>
  /// <returns>すべての文字を連結した文字列</returns>
  [[nodiscard]]
  String GetConcatenatedString() const;

private:
  Array<String> characters_;  // 獲得した文字配列
  size_t max_size_;           // 最大文字数

  // デフォルト値の定数
  static constexpr size_t kDefaultMaxSize = 7;
};
