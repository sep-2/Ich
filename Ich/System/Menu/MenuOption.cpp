#include "stdafx.h"
#include "MenuOption.h"
#include "GameSettings.h"
#include "MenuSoundManager.h"

MenuOption::MenuOption()
  : font_(30)
  , help_font_(16)
  , volume_slider_area_(440, 260, kSliderWidth, kSliderHeight)  // 画面中央に配置（1280x720）
  , brightness_slider_area_(440, 360, kSliderWidth, kSliderHeight)  // 画面中央に配置
  , back_button_(540, 510, 200, 50)  // 画面中央に配置
  , selected_item_(0) {
}

MenuOption::~MenuOption() {
}

bool MenuOption::Update() {
  auto* settings = GameSettings::GetInstance();
  auto* sound = MenuSoundManager::GetInstance();

  const int32 item_count = static_cast<int32>(OptionItem::kCount);

  // 上下キーで選択を移動
  if (KeyUp.down() || KeyW.down()) {
    sound->PlaySe(MenuSeKind::kHover);
    selected_item_--;
    if (selected_item_ < 0) {
      selected_item_ = item_count - 1;
    }
  }
  else if (KeyDown.down() || KeyS.down()) {
    sound->PlaySe(MenuSeKind::kHover);
    selected_item_++;
    if (selected_item_ >= item_count) {
      selected_item_ = 0;
    }
  }

  // 左右キーでスライダーを調整
  const OptionItem selected = static_cast<OptionItem>(selected_item_);
  
  if (selected == OptionItem::kVolume) {
    if (KeyLeft.down() || KeyA.down()) {
      double new_volume = Clamp(settings->GetVolume() - kSliderAdjustStep, 0.0, 1.0);
      settings->SetVolume(new_volume);
      sound->PlaySe(MenuSeKind::kSlider);
    }
    else if (KeyRight.down() || KeyD.down()) {
      double new_volume = Clamp(settings->GetVolume() + kSliderAdjustStep, 0.0, 1.0);
      settings->SetVolume(new_volume);
      sound->PlaySe(MenuSeKind::kSlider);
    }
  }
  else if (selected == OptionItem::kBrightness) {
    if (KeyLeft.down() || KeyA.down()) {
      double new_brightness = Clamp(settings->GetBrightness() - kSliderAdjustStep, 0.0, 1.0);
      settings->SetBrightness(new_brightness);
      sound->PlaySe(MenuSeKind::kSlider);
    }
    else if (KeyRight.down() || KeyD.down()) {
      double new_brightness = Clamp(settings->GetBrightness() + kSliderAdjustStep, 0.0, 1.0);
      settings->SetBrightness(new_brightness);
      sound->PlaySe(MenuSeKind::kSlider);
    }
  }

  // Enterキーで決定（戻るボタンが選択されている場合）
  if ((KeyEnter.down() || KeySpace.down()) && selected == OptionItem::kBack) {
    sound->PlaySe(MenuSeKind::kClick);
    return true;
  }

  // Escキーで戻る
  if (KeyEscape.down()) {
    sound->PlaySe(MenuSeKind::kClick);
    return true;
  }

  return false;
}

void MenuOption::Draw() const {
  auto* settings = GameSettings::GetInstance();

  // 背景
  Scene::Rect().draw(ColorF(0.2, 0.3, 0.4, 0.8));

  // 画面中央の座標（1280x720）
  constexpr int32 kScreenCenterX = 640;

  // タイトル
  font_(U"オプション").drawAt(kScreenCenterX, 180, Palette::White);

  // 明滅用の係数
  const double pulse = 0.5 + 0.5 * Periodic::Sine0_1(1.5s);

  // カーソルのアニメーション（左右に揺れる）
  const double cursor_sway = Sin(Scene::Time() * 4.0) * 3.0;  // 左右3pxの揺れ

  // カーソルの色（黄色で統一）
  const ColorF cursor_color = ColorF(1.0, 0.9, 0.0);

  // 現在選択されている項目
  const OptionItem selected = static_cast<OptionItem>(selected_item_);

  // 音量
  const bool is_volume_selected = (selected == OptionItem::kVolume);
  if (is_volume_selected)
  {
    // マーカーをラベルの左側に描画（黄色）
    const Vec2 cursor_pos(320 + cursor_sway, 260);
    font_(U"▶").draw(cursor_pos, cursor_color);
    font_(U"音量").draw(340, 260, Palette::Yellow);
  }
  else
  {
    font_(U"音量").draw(340, 260, Palette::White);
  }
  
  // スライダー背景（選択時は明るく）
  const ColorF slider_bg_color = is_volume_selected 
    ? ColorF(0.6) 
    : ColorF(0.5).lerp(ColorF(0.55), pulse);
  volume_slider_area_.draw(slider_bg_color);
  
  // 音量バー
  const ColorF volume_bar_color = is_volume_selected ? ColorF(Palette::Orange) : ColorF(Palette::Orange, 0.7);
  Rect(volume_slider_area_.x, volume_slider_area_.y,
       static_cast<int>(volume_slider_area_.w * settings->GetVolume()), volume_slider_area_.h)
      .draw(volume_bar_color);
  
  // 音量の値を表示
  font_(U"{:.0f}%"_fmt(settings->GetVolume() * 100))
    .draw(volume_slider_area_.x + volume_slider_area_.w + 20, 260, Palette::White);

  // 明るさ
  const bool is_brightness_selected = (selected == OptionItem::kBrightness);
  if (is_brightness_selected)
  {
    // マーカーをラベルの左側に描画（黄色）
    const Vec2 cursor_pos(320 + cursor_sway, 360);
    font_(U"▶").draw(cursor_pos, cursor_color);
    font_(U"明るさ").draw(340, 360, Palette::Yellow);
  }
  else
  {
    font_(U"明るさ").draw(340, 360, Palette::White);
  }
  
  // スライダー背景（選択時は明るく）
  const ColorF brightness_bg_color = is_brightness_selected 
    ? ColorF(0.6) 
    : ColorF(0.5).lerp(ColorF(0.55), pulse);
  brightness_slider_area_.draw(brightness_bg_color);
  
  // 明るさバー
  const ColorF brightness_bar_color = is_brightness_selected ? ColorF(Palette::Yellow) : ColorF(Palette::Yellow, 0.7);
  Rect(brightness_slider_area_.x, brightness_slider_area_.y,
       static_cast<int>(brightness_slider_area_.w * settings->GetBrightness()), brightness_slider_area_.h)
      .draw(brightness_bar_color);
  
  // 明るさの値を表示
  font_(U"{:.0f}%"_fmt(settings->GetBrightness() * 100))
    .draw(brightness_slider_area_.x + brightness_slider_area_.w + 20, 360, Palette::White);

  // 戻るボタン
  const bool is_back_selected = (selected == OptionItem::kBack);
  if (is_back_selected) {
    // 選択時: より明るく
    back_button_.draw(Palette::Lightgray);
    font_(U"戻る").drawAt(back_button_.center(), Palette::Black);
    // マーカーをボックスの左側に描画（黄色）
    const Vec2 cursor_pos(back_button_.x - 40 + cursor_sway, back_button_.y + 15);
    font_(U"▶").draw(cursor_pos, cursor_color);
  }
  else {
    // 非選択時: 明滅
    const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
    back_button_.draw(button_color);
    font_(U"戻る").drawAt(back_button_.center(), Palette::White);
  }
  
  // 操作説明
  help_font_(U"↑↓: 項目選択  ←→: 値調整  Enter/Esc: 戻る")
    .drawAt(kScreenCenterX, 620, ColorF(1.0, 1.0, 1.0, 0.7));
}
