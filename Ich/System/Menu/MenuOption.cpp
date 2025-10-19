#include "stdafx.h"
#include "MenuOption.h"
#include "GameSettings.h"
#include "MenuSoundManager.h"

MenuOption::MenuOption()
  : font_(30)
  , volume_slider_area_(200, 150, kSliderWidth, kSliderHeight)
  , brightness_slider_area_(200, 250, kSliderWidth, kSliderHeight)
  , back_button_(300, 400, 200, 50) {
}

MenuOption::~MenuOption() {
}

bool MenuOption::Update() {
  auto* settings = GameSettings::GetInstance();
  auto* sound = MenuSoundManager::GetInstance();

  static bool was_volume_hovering = false;
  static bool was_brightness_hovering = false;
  static bool was_back_hovering = false;

  // 音量スライダー
  bool volume_hovering = volume_slider_area_.mouseOver();
  if (volume_hovering && !was_volume_hovering) {
    sound->PlaySe(MenuSeKind::kHover);
  }
  was_volume_hovering = volume_hovering;

  if (volume_slider_area_.mouseOver()) {
    if (MouseL.pressed()) {
      double ratio = (Cursor::Pos().x - volume_slider_area_.x) / static_cast<double>(volume_slider_area_.w);
      double new_volume = Clamp(ratio, 0.0, 1.0);
      
      // 値が変わったときだけ音を鳴らす
      static double last_volume = -1.0;
      if (std::abs(new_volume - last_volume) > 0.01) {
        settings->SetVolume(new_volume);
        sound->PlaySe(MenuSeKind::kSlider);
        last_volume = new_volume;
      }
    }
  }

  // 明るさスライダー
  bool brightness_hovering = brightness_slider_area_.mouseOver();
  if (brightness_hovering && !was_brightness_hovering) {
    sound->PlaySe(MenuSeKind::kHover);
  }
  was_brightness_hovering = brightness_hovering;

  if (brightness_slider_area_.mouseOver()) {
    if (MouseL.pressed()) {
      double ratio = (Cursor::Pos().x - brightness_slider_area_.x) / static_cast<double>(brightness_slider_area_.w);
      double new_brightness = Clamp(ratio, 0.0, 1.0);
      
      // 値が変わったときだけ音を鳴らす
      static double last_brightness = -1.0;
      if (std::abs(new_brightness - last_brightness) > 0.01) {
        settings->SetBrightness(new_brightness);
        sound->PlaySe(MenuSeKind::kSlider);
        last_brightness = new_brightness;
      }
    }
  }

  // 戻るボタン
  bool back_hovering = back_button_.mouseOver();
  if (back_hovering && !was_back_hovering) {
    sound->PlaySe(MenuSeKind::kHover);
  }
  was_back_hovering = back_hovering;

  if (back_button_.leftClicked()) {
    sound->PlaySe(MenuSeKind::kClick);
    return true;
  }

  return false;
}

void MenuOption::Draw() const {
  auto* settings = GameSettings::GetInstance();

  // 背景
  Scene::Rect().draw(ColorF(0.2, 0.3, 0.4, 0.8));

  // タイトル
  font_(U"オプション").drawAt(400, 80, Palette::White);

  // 明滅用の係数
  const double pulse = 0.5 + 0.5 * Periodic::Sine0_1(1.5s);

  // 音量
  font_(U"音量").draw(50, 150, Palette::White);
  
  // スライダー背景（明滅）
  const ColorF slider_bg_color = volume_slider_area_.mouseOver() 
    ? ColorF(0.6) 
    : ColorF(0.5).lerp(ColorF(0.55), pulse);
  volume_slider_area_.draw(slider_bg_color);
  
  // 音量バー
  Rect(volume_slider_area_.x, volume_slider_area_.y,
       static_cast<int>(volume_slider_area_.w * settings->GetVolume()), volume_slider_area_.h)
      .draw(Palette::Orange);

  // 明るさ
  font_(U"明るさ").draw(50, 250, Palette::White);
  
  // スライダー背景（明滅）
  const ColorF brightness_bg_color = brightness_slider_area_.mouseOver() 
    ? ColorF(0.6) 
    : ColorF(0.5).lerp(ColorF(0.55), pulse);
  brightness_slider_area_.draw(brightness_bg_color);
  
  // 明るさバー
  Rect(brightness_slider_area_.x, brightness_slider_area_.y,
       static_cast<int>(brightness_slider_area_.w * settings->GetBrightness()), brightness_slider_area_.h)
      .draw(Palette::Yellow);

  // 戻るボタン
  if (back_button_.mouseOver()) {
    // ホバー時: より明るく
    back_button_.draw(Palette::Lightgray);
    font_(U"戻る").drawAt(back_button_.center(), Palette::Black);
  }
  else {
    // 通常時: 明滅
    const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
    back_button_.draw(button_color);
    font_(U"戻る").drawAt(back_button_.center(), Palette::White);
  }
}
