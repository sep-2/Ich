#pragma once
#include <Siv3D.hpp>

/// <summary>
/// オプション項目
/// </summary>
enum class OptionItem {
  kVolume,      // 音量
  kBrightness,  // 明るさ
  kBack,        // 戻る
  kCount        // 項目数
};

/// <summary>
/// オプション画面クラス
/// </summary>
class MenuOption {
public:
  MenuOption();
  ~MenuOption();

  /// <summary>
  /// 更新処理
  /// </summary>
  /// <returns>戻るボタンが押されたら true</returns>
  bool Update();

  /// <summary>
  /// 描画処理
  /// </summary>
  void Draw() const;

private:
  Font font_;
  Font help_font_;  // 操作説明用フォント
  Rect volume_slider_area_;
  Rect brightness_slider_area_;
  Rect back_button_;

  // キーボード操作用
  int32 selected_item_ = 0;  // 選択中の項目

  static constexpr int kSliderWidth = 400;
  static constexpr int kSliderHeight = 30;
  static constexpr double kSliderAdjustStep = 0.05;  // スライダー調整ステップ
};
