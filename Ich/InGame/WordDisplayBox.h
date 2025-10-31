#pragma once
#include <Siv3D.hpp>

/// <summary>
/// もじぴったん風の文字表示ボックスUI
/// 収集した文字を縦に並べて表示する
/// </summary>
class WordDisplayBox
{
public:
  WordDisplayBox();
  ~WordDisplayBox();

  /// <summary>
  /// 更新処理
  /// </summary>
  /// <param name="deltaTime">デルタタイム</param>
  void Update(double deltaTime);

  /// <summary>
  /// 描画処理
  /// </summary>
  /// <param name="blockFont">ブロック用フォント</param>
  void Draw(const Font& blockFont) const;

  /// <summary>
  /// 表示する文字配列を設定
  /// </summary>
  /// <param name="words">文字配列</param>
  void SetWords(const Array<String>& words);

  /// <summary>
  /// 完成した単語リストを設定（文字の色を変えるため）
  /// </summary>
  /// <param name="completedWords">完成した単語リスト</param>
  void SetCompletedWords(const Array<String>& completedWords);

  /// <summary>
  /// 表示位置を設定
  /// </summary>
  /// <param name="x">X座標</param>
  /// <param name="y">Y座標</param>
  void SetPosition(int32 x, int32 y);

  /// <summary>
  /// ボックスサイズを設定
  /// </summary>
  /// <param name="size">ボックスサイズ</param>
  void SetBoxSize(int32 size);

  /// <summary>
  /// ボックス間の隙間を設定
  /// </summary>
  /// <param name="spacing">隙間サイズ</param>
  void SetBoxSpacing(int32 spacing);

private:
  /// <summary>
  /// 文字が完成した単語に含まれているかチェック
  /// </summary>
  /// <param name="word">チェックする文字</param>
  /// <returns>完成した単語に含まれている場合true</returns>
  bool IsInCompletedWord(const String& word) const;

  Array<String> words_;                // 表示する文字配列
  Array<String> completed_words_;      // 完成した単語リスト
  int32 position_x_;                   // 表示位置X
  int32 position_y_;                   // 表示位置Y
  int32 box_size_;                     // ボックスサイズ
  int32 box_spacing_;                  // ボックス間の隙間

  // 定数
  static constexpr double kRoundRadius = 5.0;
  static constexpr double kFrameThickness = 3.0;
  static constexpr Vec2 kTextShadowOffset{ 2.0, 2.0 };

  static const ColorF kTextShadowColor;
  static const ColorF kDefaultTextColor;
  static const ColorF kCompletedTextColor;
  static const ColorF kDefaultBoxColor;
  static const ColorF kCompletedBoxColor;
  static const ColorF kDefaultBorderColor;
  static const ColorF kCompletedBorderColor;
};
