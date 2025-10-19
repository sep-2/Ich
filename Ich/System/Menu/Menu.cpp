#include "stdafx.h"
#include "Menu.h"
#include "MenuSoundManager.h"

Menu::Menu()
  : state_(MenuState::kNone)
  , font_(40)
  , resume_button_(250, 180, 300, 60)
  , option_button_(250, 260, 300, 60)
  , quit_button_(250, 340, 300, 60)
  , quit_yes_button_(220, 280, 150, 60)
  , quit_no_button_(430, 280, 150, 60)
  , menu_option_(std::make_unique<MenuOption>())
  , quit_requested_(false) {
  
  // 確認ダイアログ用のフォントをメンバとして初期化（Draw関数内での生成を避ける）
  message_font_ = Font(35);
}

Menu::~Menu() {
}

void Menu::Open() {
  state_ = MenuState::kMain;
}

void Menu::Close() {
  state_ = MenuState::kNone;
}

bool Menu::Update() {
  if (state_ == MenuState::kNone) {
    return false;
  }

  auto* sound = MenuSoundManager::GetInstance();
  static bool was_resume_hovering = false;
  static bool was_option_hovering = false;
  static bool was_quit_hovering = false;
  static bool was_yes_hovering = false;
  static bool was_no_hovering = false;

  if (state_ == MenuState::kMain) {
    // 戻るボタン
    bool resume_hovering = resume_button_.mouseOver();
    if (resume_hovering && !was_resume_hovering) {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_resume_hovering = resume_hovering;

    if (resume_button_.leftClicked()) {
      sound->PlaySe(MenuSeKind::kClick);
      Close();
      return true;
    }

    // オプションボタン
    bool option_hovering = option_button_.mouseOver();
    if (option_hovering && !was_option_hovering) {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_option_hovering = option_hovering;

    if (option_button_.leftClicked()) {
      sound->PlaySe(MenuSeKind::kClick);
      state_ = MenuState::kOption;
    }

    // ゲーム終了ボタン
    bool quit_hovering = quit_button_.mouseOver();
    if (quit_hovering && !was_quit_hovering) {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_quit_hovering = quit_hovering;

    if (quit_button_.leftClicked()) {
      sound->PlaySe(MenuSeKind::kClick);
      // 確認ダイアログを表示
      state_ = MenuState::kQuitConfirm;
    }

    // Esc でメニューを閉じる
    if (KeyEscape.down()) {
      sound->PlaySe(MenuSeKind::kClick);
      Close();
      return true;
    }
  }
  else if (state_ == MenuState::kOption) {
    // オプション画面の更新
    if (menu_option_->Update()) {
      state_ = MenuState::kMain;
      // ホバー状態をリセット
      was_resume_hovering = false;
      was_option_hovering = false;
      was_quit_hovering = false;
    }
  }
  else if (state_ == MenuState::kQuitConfirm) {
    // はいボタン
    bool yes_hovering = quit_yes_button_.mouseOver();
    if (yes_hovering && !was_yes_hovering) {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_yes_hovering = yes_hovering;

    if (quit_yes_button_.leftClicked()) {
      sound->PlaySe(MenuSeKind::kClick);
      quit_requested_ = true;
      Close();
      return true;
    }

    // いいえボタン
    bool no_hovering = quit_no_button_.mouseOver();
    if (no_hovering && !was_no_hovering) {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_no_hovering = no_hovering;

    if (quit_no_button_.leftClicked()) {
      sound->PlaySe(MenuSeKind::kClick);
      // メインメニューに戻る
      state_ = MenuState::kMain;
      was_yes_hovering = false;
      was_no_hovering = false;
    }

    // Esc で確認ダイアログをキャンセル
    if (KeyEscape.down()) {
      sound->PlaySe(MenuSeKind::kClick);
      state_ = MenuState::kMain;
      was_yes_hovering = false;
      was_no_hovering = false;
    }
  }

  return false;
}

void Menu::Draw() const {
  if (state_ == MenuState::kNone) {
    return;
  }

  if (state_ == MenuState::kMain) {
    // 半透明背景
    Scene::Rect().draw(ColorF(0.0, 0.0, 0.0, 0.5));

    // メニュータイトル
    font_(U"メニュー").drawAt(400, 100, Palette::White);

    // 明滅用の係数（0.5〜1.0の範囲で変化）
    const double pulse = 0.5 + 0.5 * Periodic::Sine0_1(1.5s);

    // 戻るボタン
    if (resume_button_.mouseOver()) {
      // ホバー時: より明るく
      resume_button_.draw(Palette::Lightgray);
      font_(U"戻る").drawAt(resume_button_.center(), Palette::Black);
    }
    else {
      // 通常時: 明滅
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      resume_button_.draw(button_color);
      font_(U"戻る").drawAt(resume_button_.center(), Palette::White);
    }

    // オプションボタン
    if (option_button_.mouseOver()) {
      // ホバー時: より明るく
      option_button_.draw(Palette::Lightgray);
      font_(U"オプション").drawAt(option_button_.center(), Palette::Black);
    }
    else {
      // 通常時: 明滅
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      option_button_.draw(button_color);
      font_(U"オプション").drawAt(option_button_.center(), Palette::White);
    }

    // ゲーム終了ボタン
    if (quit_button_.mouseOver()) {
      // ホバー時: 赤系で警告
      quit_button_.draw(Palette::Lightgray);
      font_(U"終了").drawAt(quit_button_.center(), Palette::Black);
    }
    else {
      // 通常時: 暗めの赤で明滅
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      quit_button_.draw(button_color);
      font_(U"終了").drawAt(quit_button_.center(), Palette::White);
    }
  }
  else if (state_ == MenuState::kOption) {
    menu_option_->Draw();
  }
  else if (state_ == MenuState::kQuitConfirm) {
    // より暗い背景
    Scene::Rect().draw(ColorF(0.0, 0.0, 0.0, 0.7));

    // 確認ダイアログボックス
    const Rect dialog_box(200, 180, 400, 180);
    dialog_box.draw(ColorF(0.2, 0.2, 0.3, 0.95));
    dialog_box.drawFrame(2, Palette::White);

    // 確認メッセージ（メンバ変数のフォントを使用）
    message_font_(U"本当に終了しますか？").drawAt(400, 230, Palette::White);

    // 明滅用の係数
    const double pulse = 0.5 + 0.5 * Periodic::Sine0_1(1.5s);

    // はいボタン
    if (quit_yes_button_.mouseOver()) {
      quit_yes_button_.draw(Palette::Lightcoral);
      font_(U"はい").drawAt(quit_yes_button_.center(), Palette::Black);
    }
    else {
      const ColorF button_color = ColorF(Palette::Darkred).lerp(Palette::Indianred, 1 - pulse);
      quit_yes_button_.draw(button_color);
      font_(U"はい").drawAt(quit_yes_button_.center(), Palette::White);
    }

    // いいえボタン
    if (quit_no_button_.mouseOver()) {
      quit_no_button_.draw(Palette::Lightgray);
      font_(U"いいえ").drawAt(quit_no_button_.center(), Palette::Black);
    }
    else {
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      quit_no_button_.draw(button_color);
      font_(U"いいえ").drawAt(quit_no_button_.center(), Palette::White);
    }
  }
}
