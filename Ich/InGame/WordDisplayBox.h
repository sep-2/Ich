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

  /// <summary>
  /// 単語完成時の拡大エフェクトを開始
  /// </summary>
  /// <param name="completedWord">完成した単語</param>
  void TriggerCompletionEffect(const String& completedWord);

private:
  /// <summary>
  /// 文字が完成した単語に含まれているかチェック
  /// </summary>
  /// <param name="word">チェックする文字</param>
  /// <returns>完成した単語に含まれている場合true</returns>
  bool IsInCompletedWord(const String& word) const;

  /// <summary>
  /// エフェクト情報（文字数ベース、下から適用）
  /// </summary>
  struct CharacterEffect
  {
    size_t char_count;      // エフェクト対象の文字数（下から数えて）
    double elapsed;         // 経過時間
    bool active;            // エフェクトが有効か
    
    CharacterEffect() : char_count(0), elapsed(0.0), active(false) {}
  };

  Array<String> words_;                // 表示する文字配列
  Array<String> completed_words_;      // 完成した単語リスト
  int32 position_x_;                   // 表示位置X
  int32 position_y_;                   // 表示位置Y
  int32 box_size_;                     // ボックスサイズ
  int32 box_spacing_;                  // ボックス間の隙間

  Array<CharacterEffect> effects_;     // 各単語のエフェクト情報

  // デフォルト値の定数
  static constexpr int32 kDefaultPositionX = 50;
  static constexpr int32 kDefaultPositionY = 20;
  static constexpr int32 kDefaultBoxSize = 50;
  static constexpr int32 kDefaultBoxSpacing = 5;

  // 描画用定数
  static constexpr double kRoundRadius = 5.0;
  static constexpr double kFrameThickness = 3.0;
  static constexpr Vec2 kTextShadowOffset{ 2.0, 2.0 };

  // エフェクト用定数
  static constexpr double kEffectExpandDuration = 0.5;    // 拡大時間（秒）
  static constexpr double kEffectShrinkDuration = 0.5;    // 縮小時間（秒）
  static constexpr double kEffectTotalDuration = 1.0;     // 全体時間（秒）
  static constexpr double kEffectMaxScale = 2.0;          // 最大スケール
  static constexpr double kEffectDelayPerChar = 0.1;      // 文字ごとの遅延（秒）

  static const ColorF kTextShadowColor;
  static const ColorF kDefaultTextColor;
  static const ColorF kCompletedTextColor;
  static const ColorF kDefaultBoxColor;
  static const ColorF kCompletedBoxColor;
  static const ColorF kDefaultBorderColor;
  static const ColorF kCompletedBorderColor;
};
