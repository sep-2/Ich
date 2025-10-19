#pragma once
#include <Siv3D.hpp>

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
  Rect volume_slider_area_;
  Rect brightness_slider_area_;
  Rect back_button_;

  static constexpr int kSliderWidth = 400;
  static constexpr int kSliderHeight = 30;
};
