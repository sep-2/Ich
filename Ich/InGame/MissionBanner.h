#pragma once
#include <Siv3D.hpp>

/// <summary>
/// ミッション開始時のバナー演出クラス（スパロボ風）
/// </summary>
class MissionBanner
{
public:
  MissionBanner();
  ~MissionBanner();

  /// <summary>
  /// 演出を開始
  /// </summary>
  /// <param name="wordCount">完成させる単語数</param>
  void Start(int32 wordCount);

  /// <summary>
  /// 更新処理
  /// </summary>
  /// <param name="deltaTime">デルタタイム</param>
  void Update(double deltaTime);

  /// <summary>
  /// 描画処理
  /// </summary>
  void Draw() const;

  /// <summary>
  /// 演出が完了したかどうか
  /// </summary>
  /// <returns>完了していればtrue</returns>
  bool IsFinished() const;

private:
  enum class Phase
  {
    kIdle,          // 待機中
    kBannerExpand,  // 黒い帯が広がる
    kTextFadeIn,    // テキストがフェードイン
    kTextDisplay,   // テキスト表示中（静止）
    kTextFadeOut,   // テキストがフェードアウト
    kBannerShrink,  // 黒い帯が縮む
    kFinished       // 演出終了
  };

  Phase phase_;
  double elapsed_time_;
  int32 word_count_;
  String mission_text_;
  Font font_;

  // 各フェーズの時間設定（秒）
  static constexpr double kBannerExpandDuration = 0.5;
  static constexpr double kTextFadeInDuration = 0.5;
  static constexpr double kTextDisplayDuration = 3.0;
  static constexpr double kTextFadeOutDuration = 0.5;
  static constexpr double kBannerShrinkDuration = 0.5;

  // バナーの最大高さ（ピクセル）
  static constexpr double kMaxBannerHeight = 120.0;
};
