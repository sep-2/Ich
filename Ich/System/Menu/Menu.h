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
/// メインメニューの選択肢
/// </summary>
enum class MainMenuItem {
  kResume,      // 戻る
#if _DEBUG
  kRestart,     // 再起動
  kInfiniteAir, // エア無限
#endif
  kOption,      // オプション
  kReturnToTitle, // タイトルに戻る
  kQuit,        // 終了
  kCount        // 項目数
};

/// <summary>
/// 確認ダイアログの種類
/// </summary>
enum class ConfirmDialogType {
  kNone,          // なし
  kQuit,          // 終了確認
  kReturnToTitle  // タイトルに戻る確認
};

/// <summary>
/// 終了確認ダイアログの選択肢
/// </summary>
enum class QuitConfirmItem {
  kNo,    // いいえ（左側、デフォルト選択）
  kYes,   // はい（右側）
  kCount  // 項目数
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

  /// <summary>
  /// タイトルに戻るがリクエストされたか
  /// </summary>
  bool IsReturnToTitleRequested() const { return return_to_title_requested_; }

#if _DEBUG
  /// <summary>
  /// ゲーム再起動がリクエストされたか
  /// </summary>
  bool IsRestartRequested() const { return restart_requested_; }

  /// <summary>
  /// エア無限フラグを取得
  /// </summary>
  bool IsInfiniteAirEnabled() const { return infinite_air_; }
#endif

private:
  MenuState state_;
  Font font_;
  Font message_font_;  // 確認ダイアログ用フォント
  Rect resume_button_;
  Rect option_button_;
  Rect return_to_title_button_;  // タイトルに戻るボタン
  Rect quit_button_;
  
#if _DEBUG
  Rect restart_button_;  // ゲーム再起動ボタン（DEBUGのみ）
  Rect infinite_air_toggle_;  // エア無限トグルボタン（DEBUGのみ）
  bool infinite_air_ = false;  // エア無限フラグ
#endif
  
  // 終了確認ダイアログ用
  Rect quit_yes_button_;
  Rect quit_no_button_;
  ConfirmDialogType current_confirm_dialog_ = ConfirmDialogType::kNone;  // 現在表示中の確認ダイアログ
  
  std::unique_ptr<MenuOption> menu_option_;
  bool quit_requested_ = false;
  bool return_to_title_requested_ = false;  // タイトルに戻るフラグ

#if _DEBUG
  bool restart_requested_ = false;
#endif

  // キーボード操作用
  int32 selected_main_item_ = 0;  // メインメニューの選択中の項目
  int32 selected_quit_item_ = 0;  // 終了確認ダイアログの選択中の項目
};
