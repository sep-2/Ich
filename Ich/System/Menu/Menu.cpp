#include "stdafx.h"
#include "Menu.h"
#include "MenuSoundManager.h"

namespace MenuConstants
{
  // 画面サイズ
  constexpr int32 kScreenWidth = 1280;
  constexpr int32 kScreenHeight = 720;
  
  // メニュー位置・サイズ
  constexpr int32 kMenuCenterX = kScreenWidth / 2;  // 640
  constexpr int32 kMenuTitleY = 150;
  constexpr int32 kButtonWidth = 300;
  constexpr int32 kButtonHeight = 60;
  constexpr int32 kButtonX = (kScreenWidth - kButtonWidth) / 2;  // 490（画面中央に配置）
  constexpr int32 kCursorOffsetX = -40;  // ▶マーカーのオフセット（ボックスの左側に表示）
  
  // ボタンY座標（DEBUG/RELEASE共通）
  constexpr int32 kResumeButtonY = 230;
  
  // DEBUG専用ボタンY座標
#if _DEBUG
  constexpr int32 kRestartButtonY = 310;
  constexpr int32 kInfiniteAirToggleY = 390;
  constexpr int32 kOptionButtonYDebug = 470;
  constexpr int32 kReturnToTitleButtonYDebug = 550;
  constexpr int32 kQuitButtonYDebug = 630;
#else
  constexpr int32 kOptionButtonYRelease = 310;
  constexpr int32 kReturnToTitleButtonYRelease = 390;
  constexpr int32 kQuitButtonYRelease = 470;
#endif
  
  // 終了確認ダイアログ
  constexpr int32 kDialogWidth = 400;
  constexpr int32 kDialogHeight = 180;
  constexpr int32 kDialogX = (kScreenWidth - kDialogWidth) / 2;  // 440（画面中央に配置）
  constexpr int32 kDialogY = (kScreenHeight - kDialogHeight) / 2;  // 270（画面中央に配置）
  constexpr int32 kDialogMessageY = kDialogY + 50;
  constexpr int32 kDialogButtonWidth = 150;
  constexpr int32 kDialogButtonHeight = 60;
  constexpr int32 kDialogNoButtonX = kDialogX + 30;   // いいえボタン（左側）
  constexpr int32 kDialogYesButtonX = kDialogX + kDialogWidth - kDialogButtonWidth - 30;  // はいボタン（右側）
  constexpr int32 kDialogButtonY = kDialogY + kDialogHeight - kDialogButtonHeight - 20;
  
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
  , return_to_title_button_(MenuConstants::kButtonX, MenuConstants::kReturnToTitleButtonYDebug, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
  , quit_button_(MenuConstants::kButtonX, MenuConstants::kQuitButtonYDebug, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
  , infinite_air_(false)
#else
  , option_button_(MenuConstants::kButtonX, MenuConstants::kOptionButtonYRelease, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
  , return_to_title_button_(MenuConstants::kButtonX, MenuConstants::kReturnToTitleButtonYRelease, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
  , quit_button_(MenuConstants::kButtonX, MenuConstants::kQuitButtonYRelease, MenuConstants::kButtonWidth, MenuConstants::kButtonHeight)
#endif
  , quit_no_button_(MenuConstants::kDialogNoButtonX, MenuConstants::kDialogButtonY, MenuConstants::kDialogButtonWidth, MenuConstants::kDialogButtonHeight)
  , quit_yes_button_(MenuConstants::kDialogYesButtonX, MenuConstants::kDialogButtonY, MenuConstants::kDialogButtonWidth, MenuConstants::kDialogButtonHeight)
  , current_confirm_dialog_(ConfirmDialogType::kNone)
  , menu_option_(std::make_unique<MenuOption>())
  , quit_requested_(false)
  , return_to_title_requested_(false)
#if _DEBUG
  , restart_requested_(false)
#endif
  , selected_main_item_(0)
  , selected_quit_item_(0)
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
  selected_main_item_ = 0;  // 選択をリセット
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

  if (state_ == MenuState::kMain)
  {
    // メインメニューの項目数を計算
#if _DEBUG
    const int32 item_count = static_cast<int32>(MainMenuItem::kCount);
#else
    const int32 item_count = 4; // 戻る、オプション、タイトルに戻る、終了
#endif

    // 上下キーで選択を移動
    if (KeyUp.down() || KeyW.down())
    {
      sound->PlaySe(MenuSeKind::kHover);
      selected_main_item_--;
      if (selected_main_item_ < 0)
      {
        selected_main_item_ = item_count - 1;
      }
    }
    else if (KeyDown.down() || KeyS.down())
    {
      sound->PlaySe(MenuSeKind::kHover);
      selected_main_item_++;
      if (selected_main_item_ >= item_count)
      {
        selected_main_item_ = 0;
      }
    }

    // Enterキーで決定
    if (KeyEnter.down() || KeySpace.down())
    {
      sound->PlaySe(MenuSeKind::kClick);

#if _DEBUG
      const MainMenuItem selected = static_cast<MainMenuItem>(selected_main_item_);
      
      switch (selected)
      {
      case MainMenuItem::kResume:
        Close();
        return true;
        
      case MainMenuItem::kRestart:
        restart_requested_ = true;
        Close();
        return true;
        
      case MainMenuItem::kInfiniteAir:
        infinite_air_ = !infinite_air_;
        break;
        
      case MainMenuItem::kOption:
        state_ = MenuState::kOption;
        break;
        
      case MainMenuItem::kReturnToTitle:
        state_ = MenuState::kQuitConfirm;
        current_confirm_dialog_ = ConfirmDialogType::kReturnToTitle;
        selected_quit_item_ = 0;  // 確認ダイアログの選択をリセット
        break;
        
      case MainMenuItem::kQuit:
        state_ = MenuState::kQuitConfirm;
        current_confirm_dialog_ = ConfirmDialogType::kQuit;
        selected_quit_item_ = 0;  // 終了確認ダイアログの選択をリセット
        break;
        
      default:
        break;
      }
#else
      // RELEASEビルドでは項目数が異なる
      switch (selected_main_item_)
      {
      case 0: // 戻る
        Close();
        return true;
        
      case 1: // オプション
        state_ = MenuState::kOption;
        break;
        
      case 2: // タイトルに戻る
        state_ = MenuState::kQuitConfirm;
        current_confirm_dialog_ = ConfirmDialogType::kReturnToTitle;
        selected_quit_item_ = 0;
        break;
        
      case 3: // 終了
        state_ = MenuState::kQuitConfirm;
        current_confirm_dialog_ = ConfirmDialogType::kQuit;
        selected_quit_item_ = 0;
        break;
        
      default:
        break;
      }
#endif
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
    }
  }
  else if (state_ == MenuState::kQuitConfirm)
  {
    const int32 quit_item_count = static_cast<int32>(QuitConfirmItem::kCount);

    // 左右キーで選択を移動
    if (KeyLeft.down() || KeyA.down() || KeyUp.down() || KeyW.down())
    {
      sound->PlaySe(MenuSeKind::kHover);
      selected_quit_item_--;
      if (selected_quit_item_ < 0)
      {
        selected_quit_item_ = quit_item_count - 1;
      }
    }
    else if (KeyRight.down() || KeyD.down() || KeyDown.down() || KeyS.down())
    {
      sound->PlaySe(MenuSeKind::kHover);
      selected_quit_item_++;
      if (selected_quit_item_ >= quit_item_count)
      {
        selected_quit_item_ = 0;
      }
    }

    // Enterキーで決定
    if (KeyEnter.down() || KeySpace.down())
    {
      sound->PlaySe(MenuSeKind::kClick);

      const QuitConfirmItem selected = static_cast<QuitConfirmItem>(selected_quit_item_);
      
      if (selected == QuitConfirmItem::kYes)
      {
        if (current_confirm_dialog_ == ConfirmDialogType::kQuit)
        {
          quit_requested_ = true;
        }
        else if (current_confirm_dialog_ == ConfirmDialogType::kReturnToTitle)
        {
          return_to_title_requested_ = true;
        }
        Close();
        return true;
      }
      else // kNo
      {
        state_ = MenuState::kMain;
        current_confirm_dialog_ = ConfirmDialogType::kNone;
      }
    }

    // Esc で確認ダイアログをキャンセル
    if (KeyEscape.down())
    {
      sound->PlaySe(MenuSeKind::kClick);
      state_ = MenuState::kMain;
      current_confirm_dialog_ = ConfirmDialogType::kNone;
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

    // カーソルのアニメーション（左右に揺れる）
    const double cursor_sway = Sin(Scene::Time() * 4.0) * 3.0;  // 左右3pxの揺れ

    // カーソルの色（黄色で統一）
    const ColorF cursor_color = ColorF(1.0, 0.9, 0.0);

    // 現在選択されている項目
    int32 current_item = 0;

    // 戻るボタン
    const bool is_resume_selected = (selected_main_item_ == current_item);
    if (is_resume_selected)
    {
      // 選択時: より明るく
      resume_button_.draw(Palette::Lightgray);
      font_(U"戻る").drawAt(resume_button_.center(), Palette::Black);
      // マーカーをボックスの左側に描画（黄色）
      const Vec2 cursor_pos(resume_button_.x + MenuConstants::kCursorOffsetX + cursor_sway, resume_button_.y + 15);
      font_(U"▶").draw(cursor_pos, cursor_color);
    }
    else
    {
      // 非選択時: 明滅
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      resume_button_.draw(button_color);
      font_(U"戻る").drawAt(resume_button_.center(), Palette::White);
    }
    current_item++;

#if _DEBUG
    // ゲーム再起動ボタン（DEBUGのみ）
    const bool is_restart_selected = (selected_main_item_ == current_item);
    if (is_restart_selected)
    {
      // 選択時: より明るく
      restart_button_.draw(Palette::Lightblue);
      font_(U"再起動").drawAt(restart_button_.center(), Palette::Black);
      // マーカーをボックスの左側に描画（黄色）
      const Vec2 cursor_pos(restart_button_.x + MenuConstants::kCursorOffsetX + cursor_sway, restart_button_.y + 15);
      font_(U"▶").draw(cursor_pos, cursor_color);
    }
    else
    {
      // 非選択時: 青系で明滅
      const ColorF button_color = ColorF(Palette::Darkblue).lerp(Palette::Blue, pulse);
      restart_button_.draw(button_color);
      font_(U"再起動").drawAt(restart_button_.center(), Palette::White);
    }
    current_item++;

    // エア無限トグルボタン（DEBUGのみ）
    const bool is_infinite_air_selected = (selected_main_item_ == current_item);
    const String toggle_text = infinite_air_ ? U"エア無限: ON" : U"エア無限: OFF";
    
    if (is_infinite_air_selected)
    {
      infinite_air_toggle_.draw(Palette::Lightgray);
      font_(toggle_text).drawAt(infinite_air_toggle_.center(), Palette::Black);
      // マーカーをボックスの左側に描画（黄色）
      const Vec2 cursor_pos(infinite_air_toggle_.x + MenuConstants::kCursorOffsetX + cursor_sway, infinite_air_toggle_.y + 15);
      font_(U"▶").draw(cursor_pos, cursor_color);
    }
    else
    {
      const ColorF button_color = infinite_air_ 
        ? ColorF(Palette::Darkgreen).lerp(Palette::Green, pulse)
        : ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      infinite_air_toggle_.draw(button_color);
      font_(toggle_text).drawAt(infinite_air_toggle_.center(), Palette::White);
    }
    current_item++;
#endif

    // オプションボタン
    const bool is_option_selected = (selected_main_item_ == current_item);
    if (is_option_selected)
    {
      // 選択時: より明るく
      option_button_.draw(Palette::Lightgray);
      font_(U"オプション").drawAt(option_button_.center(), Palette::Black);
      // マーカーをボックスの左側に描画（黄色）
      const Vec2 cursor_pos(option_button_.x + MenuConstants::kCursorOffsetX + cursor_sway, option_button_.y + 15);
      font_(U"▶").draw(cursor_pos, cursor_color);
    }
    else
    {
      // 非選択時: 明滅
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      option_button_.draw(button_color);
      font_(U"オプション").drawAt(option_button_.center(), Palette::White);
    }
    current_item++;

    // タイトルに戻るボタン
    const bool is_return_selected = (selected_main_item_ == current_item);
    if (is_return_selected)
    {
      // 選択時: より明るく
      return_to_title_button_.draw(Palette::Lightyellow);
      font_(U"タイトルに戻る").drawAt(return_to_title_button_.center(), Palette::Black);
      // マーカーをボックスの左側に描画（黄色）
      const Vec2 cursor_pos(return_to_title_button_.x + MenuConstants::kCursorOffsetX + cursor_sway, return_to_title_button_.y + 15);
      font_(U"▶").draw(cursor_pos, cursor_color);
    }
    else
    {
      // 非選択時: 明滅
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      return_to_title_button_.draw(button_color);
      font_(U"タイトルに戻る").drawAt(return_to_title_button_.center(), Palette::White);
    }
    current_item++;

    // ゲーム終了ボタン
    const bool is_quit_selected = (selected_main_item_ == current_item);
    if (is_quit_selected)
    {
      // 選択時: 赤系で警告
      quit_button_.draw(Palette::Lightgray);
      font_(U"終了").drawAt(quit_button_.center(), Palette::Black);
      // マーカーをボックスの左側に描画（黄色）
      const Vec2 cursor_pos(quit_button_.x + MenuConstants::kCursorOffsetX + cursor_sway, quit_button_.y + 15);
      font_(U"▶").draw(cursor_pos, cursor_color);
    }
    else
    {
      // 非選択時: 暗めの赤で明滅
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
    String message = U"本当に終了しますか？";
    if (current_confirm_dialog_ == ConfirmDialogType::kReturnToTitle)
    {
      message = U"タイトルに戻りますか？";
    }
    message_font_(message).drawAt(MenuConstants::kMenuCenterX, MenuConstants::kDialogMessageY, Palette::White);

    // 明滅用の係数
    const double pulse = MenuConstants::kPulseMin + (MenuConstants::kPulseMax - MenuConstants::kPulseMin) * Periodic::Sine0_1(MenuConstants::kPulseDuration);

    // カーソルのアニメーション（左右に揺れる）
    const double cursor_sway = Sin(Scene::Time() * 4.0) * 3.0;  // 左右3pxの揺れ

    // カーソルの色（黄色で統一）
    const ColorF cursor_color = ColorF(1.0, 0.9, 0.0);

    // いいえボタン（左側、先頭）
    const bool is_no_selected = (selected_quit_item_ == static_cast<int32>(QuitConfirmItem::kNo));
    if (is_no_selected)
    {
      quit_no_button_.draw(Palette::Lightgray);
      font_(U"いいえ").drawAt(quit_no_button_.center(), Palette::Black);
      // マーカーをボックスの左側に描画（黄色）
      const Vec2 cursor_pos(quit_no_button_.x - 40 + cursor_sway, quit_no_button_.y + 15);
      font_(U"▶").draw(cursor_pos, cursor_color);
    }
    else
    {
      const ColorF button_color = ColorF(Palette::Darkgray).lerp(Palette::Gray, pulse);
      quit_no_button_.draw(button_color);
      font_(U"いいえ").drawAt(quit_no_button_.center(), Palette::White);
    }

    // はいボタン（右側）
    const bool is_yes_selected = (selected_quit_item_ == static_cast<int32>(QuitConfirmItem::kYes));
    if (is_yes_selected)
    {
      quit_yes_button_.draw(Palette::Lightcoral);
      font_(U"はい").drawAt(quit_yes_button_.center(), Palette::Black);
      // マーカーをボックスの左側に描画（黄色）
      const Vec2 cursor_pos(quit_yes_button_.x - 40 + cursor_sway, quit_yes_button_.y + 15);
      font_(U"▶").draw(cursor_pos, cursor_color);
    }
    else
    {
      const ColorF button_color = ColorF(Palette::Darkred).lerp(Palette::Indianred, 1 - pulse);
      quit_yes_button_.draw(button_color);
      font_(U"はい").drawAt(quit_yes_button_.center(), Palette::White);
    }
  }
}
