#include "stdafx.h"
#include "Menu.h"
#include "MenuSoundManager.h"

namespace MenuConstants
{
  // メニュー位置・サイズ
  constexpr int32 kMenuCenterX = 400;
  constexpr int32 kMenuTitleY = 100;
  constexpr int32 kButtonWidth = 300;
  constexpr int32 kButtonHeight = 60;
  constexpr int32 kButtonX = 250;
  
  // ボタンY座標（DEBUG/RELEASE共通）
  constexpr int32 kResumeButtonY = 180;
  
  // DEBUG専用ボタンY座標
#if _DEBUG
  constexpr int32 kRestartButtonY = 260;
  constexpr int32 kInfiniteAirToggleY = 340;
  constexpr int32 kOptionButtonYDebug = 420;
  constexpr int32 kQuitButtonYDebug = 500;
#else
  constexpr int32 kOptionButtonYRelease = 260;
  constexpr int32 kQuitButtonYRelease = 340;
#endif
  
  // 終了確認ダイアログ
  constexpr int32 kDialogX = 200;
  constexpr int32 kDialogY = 180;
  constexpr int32 kDialogWidth = 400;
  constexpr int32 kDialogHeight = 180;
  constexpr int32 kDialogMessageY = 230;
  constexpr int32 kDialogButtonWidth = 150;
  constexpr int32 kDialogButtonHeight = 60;
  constexpr int32 kDialogYesButtonX = 220;
  constexpr int32 kDialogNoButtonX = 430;
  constexpr int32 kDialogButtonY = 280;
  
  // フォントサイズ
  constexpr int32 kMainFontSize = 40;
  constexpr int32 kMessageFontSize = 35;
  
  // 色・透明度
  const ColorF kBackgroundColor{ 0.0, 0.0, 0.0, 0.5 };
  const ColorF kDialogBackgroundColor{ 0.0, 0.0, 0.0, 0.7 };
  const ColorF kDialogBoxColor{ 0.2, 0.2, 0.3, 0.95 };
  constexpr double kDialogBorderThickness = 2.0;
  
  // 明滅設定
  constexpr double kPulseMin = 0.5;
  constexpr double kPulseMax = 1.0;
  const Duration kPulseDuration = 1.5s;
}

Menu::Menu()
  : state_(MenuState::kNone)
  , font_(MenuConstants::kMainFontSize)
  , resume_button_(MenuConstants::kButtonX, MenuConstants::kResumeButtonY, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
#if _DEBUG
  , restart_button_(MenuConstants::kButtonX, MenuConstants::kRestartButtonY, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
  , infinite_air_toggle_(MenuConstants::kButtonX, MenuConstants::kInfiniteAirToggleY, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
  , option_button_(MenuConstants::kButtonX, MenuConstants::kOptionButtonYDebug, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
  , quit_button_(MenuConstants::kButtonX, MenuConstants::kQuitButtonYDebug, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
  , infinite_air_(false)
#else
  , option_button_(MenuConstants::kButtonX, MenuConstants::kOptionButtonYRelease, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
  , quit_button_(MenuConstants::kButtonX, MenuConstants::kQuitButtonYRelease, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
#endif
  , quit_yes_button_(MenuConstants::kDialogYesButtonX, MenuConstants::kDialogButtonY, MenuConstants::kDialogButtonWidth, MenuConstants::kDialogButtonHeight)
  , quit_no_button_(MenuConstants::kDialogNoButtonX, MenuConstants::kDialogButtonY, MenuConstants::kDialogButtonWidth, MenuConstants::kDialogButtonHeight)
  , menu_option_(std::make_unique<MenuOption>())
  , quit_requested_(false)
#if _DEBUG
  , restart_requested_(false)
#endif
{
  
  // 確認ダイアログ用のフォントをメンバとして初期化（Draw関数内での生成を避ける）
  message_font_ = Font(MenuConstants::kMessageFontSize);
}

Menu::~Menu()
{
}

void Menu::Open()
{
  state_ = MenuState::kMain;
}

void Menu::Close()
{
  state_ = MenuState::kNone;
}

bool Menu::Update()
{
  if (state_ == MenuState::kNone)
  {
    return false;
  }

  auto* sound = MenuSoundManager::GetInstance();
  static bool was_resume_hovering = false;
  static bool was_option_hovering = false;
  static bool was_quit_hovering = false;
  static bool was_yes_hovering = false;
  static bool was_no_hovering = false;
#if _DEBUG
  static bool was_restart_hovering = false;
  static bool was_infinite_air_hovering = false;
#endif

  if (state_ == MenuState::kMain)
  {
    // 戻るボタン
    bool resume_hovering = resume_button_.mouseOver();
    if (resume_hovering && !was_resume_hovering)
    {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_resume_hovering = resume_hovering;

    if (resume_button_.leftClicked())
    {
      sound->PlaySe(MenuSeKind::kClick);
      Close();
      return true;
    }

#if _DEBUG
    // ゲーム再起動ボタン（DEBUGのみ）
    bool restart_hovering = restart_button_.mouseOver();
    if (restart_hovering && !was_restart_hovering)
    {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_restart_hovering = restart_hovering;

    if (restart_button_.leftClicked())
    {
      sound->PlaySe(MenuSeKind::kClick);
      restart_requested_ = true;
      Close();
      return true;
    }

    // エア無限トグルボタン（DEBUGのみ）
    bool infinite_air_hovering = infinite_air_toggle_.mouseOver();
    if (infinite_air_hovering && !was_infinite_air_hovering)
    {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_infinite_air_hovering = infinite_air_hovering;

    if (infinite_air_toggle_.leftClicked())
    {
      sound->PlaySe(MenuSeKind::kClick);
      infinite_air_ = !infinite_air_;
    }
#endif

    // オプションボタン
    bool option_hovering = option_button_.mouseOver();
    if (option_hovering && !was_option_hovering)
    {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_option_hovering = option_hovering;

    if (option_button_.leftClicked())
    {
      sound->PlaySe(MenuSeKind::kClick);
      state_ = MenuState::kOption;
    }

    // ゲーム終了ボタン
    bool quit_hovering = quit_button_.mouseOver();
    if (quit_hovering && !was_quit_hovering)
    {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_quit_hovering = quit_hovering;

    if (quit_button_.leftClicked())
    {
      sound->PlaySe(MenuSeKind::kClick);
      // 確認ダイアログを表示
      state_ = MenuState::kQuitConfirm;
    }

    // Esc でメニューを閉じる
    if (KeyEscape.down())
    {
      sound->PlaySe(MenuSeKind::kClick);
      Close();
      return true;
    }
  }
  else if (state_ == MenuState::kOption)
  {
    // オプション画面の更新
    if (menu_option_->Update())
    {
      state_ = MenuState::kMain;
      // ホバー状態をリセット
      was_resume_hovering = false;
      was_option_hovering = false;
      was_quit_hovering = false;
#if _DEBUG
      was_restart_hovering = false;
      was_infinite_air_hovering = false;
#endif
    }
  }
  else if (state_ == MenuState::kQuitConfirm)
  {
    // はいボタン
    bool yes_hovering = quit_yes_button_.mouseOver();
    if (yes_hovering && !was_yes_hovering)
    {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_yes_hovering = yes_hovering;

    if (quit_yes_button_.leftClicked())
    {
      sound->PlaySe(MenuSeKind::kClick);
      quit_requested_ = true;
      Close();
      return true;
    }

    // いいえボタン
    bool no_hovering = quit_no_button_.mouseOver();
    if (no_hovering && !was_no_hovering)
    {
      sound->PlaySe(MenuSeKind::kHover);
    }
    was_no_hovering = no_hovering;

    if (quit_no_button_.leftClicked())
    {
      sound->PlaySe(MenuSeKind::kClick);
      // メインメニューに戻る
      state_ = MenuState::kMain;
      was_yes_hovering = false;
      was_no_hovering = false;
    }

    // Esc で確認ダイアログをキャンセル
    if (KeyEscape.down())
    {
      sound->PlaySe(MenuSeKind::kClick);
      state_ = MenuState::kMain;
      was_yes_hovering = false;
      was_no_hovering = false;
    }
  }

  return false;
}

void Menu::Draw() const
{
  if (state_ == MenuState::kNone)
  {
    return;
  }

  if (state_ == MenuState::kMain)
  {
    // 半透明背景
    Scene::Rect().draw(MenuConstants::kBackgroundColor);

    // メニュータイトル
    font_(U"メニュー").drawAt(MenuConstants::kMenuCenterX, MenuConstants::kMenuTitleY, Palette::White);

    // 明滅用の係数（0.5〜1.0の範囲で変化）
    const double pulse = MenuConstants::kPulseMin + (MenuConstants::kPulseMax - MenuConstants::kPulseMin) * Periodic::Sine0_1(MenuConstants::kPulseDuration);

    // 戻るボタン
    if (resume_button_.mouseOver())
    {
      // ホバー時: より明るく
      resume_button_.draw(Palette::Lightgray);
      font_(U"戻る").drawAt(resume_button_.center(), Palette::Black);
    }
    else
    {
      // 通常時: 明滅
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      resume_button_.draw(button_color);
      font_(U"戻る").drawAt(resume_button_.center(), Palette::White);
    }

#if _DEBUG
    // ゲーム再起動ボタン（DEBUGのみ）
    if (restart_button_.mouseOver())
    {
      // ホバー時: より明るく
      restart_button_.draw(Palette::Lightblue);
      font_(U"再起動").drawAt(restart_button_.center(), Palette::Black);
    }
    else
    {
      // 通常時: 青系で明滅
      const ColorF button_color = ColorF(Palette::Darkblue).lerp(Palette::Blue, pulse);
      restart_button_.draw(button_color);
      font_(U"再起動").drawAt(restart_button_.center(), Palette::White);
    }

    // エア無限トグルボタン（DEBUGのみ）
    if (infinite_air_toggle_.mouseOver())
    {
      infinite_air_toggle_.draw(Palette::Lightgray);
      const String toggle_text = infinite_air_ ? U"エア無限: ON" : U"エア無限: OFF";
      font_(toggle_text).drawAt(infinite_air_toggle_.center(), Palette::Black);
    }
    else
    {
      const ColorF button_color = infinite_air_ 
        ? ColorF(Palette::Darkgreen).lerp(Palette::Green, pulse)
        : ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      infinite_air_toggle_.draw(button_color);
      const String toggle_text = infinite_air_ ? U"エア無限: ON" : U"エア無限: OFF";
      font_(toggle_text).drawAt(infinite_air_toggle_.center(), Palette::White);
    }
#endif

    // オプションボタン
    if (option_button_.mouseOver())
    {
      // ホバー時: より明るく
      option_button_.draw(Palette::Lightgray);
      font_(U"オプション").drawAt(option_button_.center(), Palette::Black);
    }
    else
    {
      // 通常時: 明滅
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      option_button_.draw(button_color);
      font_(U"オプション").drawAt(option_button_.center(), Palette::White);
    }

    // ゲーム終了ボタン
    if (quit_button_.mouseOver())
    {
      // ホバー時: 赤系で警告
      quit_button_.draw(Palette::Lightgray);
      font_(U"終了").drawAt(quit_button_.center(), Palette::Black);
    }
    else
    {
      // 通常時: 暗めの赤で明滅
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      quit_button_.draw(button_color);
      font_(U"終了").drawAt(quit_button_.center(), Palette::White);
    }
  }
  else if (state_ == MenuState::kOption)
  {
    menu_option_->Draw();
  }
  else if (state_ == MenuState::kQuitConfirm)
  {
    // より暗い背景
    Scene::Rect().draw(MenuConstants::kDialogBackgroundColor);

    // 確認ダイアログボックス
    const Rect dialog_box(MenuConstants::kDialogX, MenuConstants::kDialogY, MenuConstants::kDialogWidth, MenuConstants::kDialogHeight);
    dialog_box.draw(MenuConstants::kDialogBoxColor);
    dialog_box.drawFrame(MenuConstants::kDialogBorderThickness, Palette::White);

    // 確認メッセージ（メンバ変数のフォントを使用）
    message_font_(U"本当に終了しますか？").drawAt(MenuConstants::kMenuCenterX, MenuConstants::kDialogMessageY, Palette::White);

    // 明滅用の係数
    const double pulse = MenuConstants::kPulseMin + (MenuConstants::kPulseMax - MenuConstants::kPulseMin) * Periodic::Sine0_1(MenuConstants::kPulseDuration);

    // はいボタン
    if (quit_yes_button_.mouseOver())
    {
      quit_yes_button_.draw(Palette::Lightcoral);
      font_(U"はい").drawAt(quit_yes_button_.center(), Palette::Black);
    }
    else
    {
      const ColorF button_color = ColorF(Palette::Darkred).lerp(Palette::Indianred, 1 - pulse);
      quit_yes_button_.draw(button_color);
      font_(U"はい").drawAt(quit_yes_button_.center(), Palette::White);
    }

    // いいえボタン
    if (quit_no_button_.mouseOver())
    {
      quit_no_button_.draw(Palette::Lightgray);
      font_(U"いいえ").drawAt(quit_no_button_.center(), Palette::Black);
    }
    else
    {
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      quit_no_button_.draw(button_color);
      font_(U"いいえ").drawAt(quit_no_button_.center(), Palette::White);
    }
  }
}
