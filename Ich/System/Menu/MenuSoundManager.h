#pragma once

#include <Siv3D.hpp>

/// <summary>
/// メニュー用効果音の種類
/// </summary>
enum class MenuSeKind {
  kHover,      // ボタンにフォーカス
  kClick,      // ボタンクリック
  kSlider,     // スライダー変更
};

/// <summary>
/// メニュー効果音マネージャ
/// </summary>
class MenuSoundManager {
public:
  static MenuSoundManager* GetInstance();

  static void Destroy() {
    if (instance_ != nullptr) {
      instance_ = nullptr;
    }
  }

  MenuSoundManager();
  ~MenuSoundManager();

  /// <summary>
  /// 効果音を再生
  /// </summary>
  void PlaySe(MenuSeKind kind);

private:
  static std::shared_ptr<MenuSoundManager> instance_;

  // GMInstrument を使った簡易音源
  Audio hover_sound_;
  Audio click_sound_;
  Audio slider_sound_;
};
