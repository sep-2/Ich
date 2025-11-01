#pragma once
#include "Scenes/Enum.h"
#include "System/SaveData/SaveData.hpp"

// タイトルシーン
class Title : public SceneManager<EnumScene, SaveData>::Scene
{
public:

  Title(const InitData& init);

  ~Title();

  // 更新関数
  void update() override;

  // 描画関数
  void draw() const override;

  void drawFadeIn(double t) const override;

  void drawFadeOut(double t) const override;

private:
  // メニュー項目
  enum class MenuItem
  {
    kGameStart,   // ゲーム開始
    kCredits,     // クレジット
    kExit         // ゲーム終了
  };

  // タイトルロゴ用フォント
  Font title_font_;
  
  // サブタイトル用フォント
  Font subtitle_font_;
  
  // メニュー用フォント
  Font menu_font_;
  
  // アニメーション用タイマー
  Stopwatch stopwatch_;
  
  // 現在選択中のメニュー項目
  MenuItem current_menu_item_;
  
  // クレジット表示フラグ
  bool show_credits_;
};
