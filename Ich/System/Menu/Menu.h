#pragma once
#include <memory>
#include <Siv3D.hpp>
#include "MenuOption.h"

/// <summary>
/// メニュー状態
/// </summary>
enum class MenuState {
  kNone,      // 非表示
  kMain,      // メインメニュー
  kOption,    // オプション
  kQuitConfirm // 終了確認ダイアログ
};

/// <summary>
/// メニュークラス
/// </summary>
class Menu {
public:
  Menu();
  ~Menu();

  /// <summary>
  /// メニューを開く
  /// </summary>
  void Open();

  /// <summary>
  /// メニューを閉じる
  /// </summary>
  void Close();

  /// <summary>
  /// メニューが開いているか
  /// </summary>
  bool IsOpen() const { return state_ != MenuState::kNone; }

  /// <summary>
  /// 更新処理
  /// </summary>
  /// <returns>ゲームに戻る場合 true</returns>
  bool Update();

  /// <summary>
  /// 描画処理
  /// </summary>
  void Draw() const;

  /// <summary>
  /// ゲーム終了がリクエストされたか
  /// </summary>
  bool IsQuitRequested() const { return quit_requested_; }

private:
  MenuState state_;
  Font font_;
  Font message_font_;  // 確認ダイアログ用フォント
  Rect resume_button_;
  Rect option_button_;
  Rect quit_button_;
  
  // 終了確認ダイアログ用
  Rect quit_yes_button_;
  Rect quit_no_button_;
  
  std::unique_ptr<MenuOption> menu_option_;
  bool quit_requested_ = false;
};
