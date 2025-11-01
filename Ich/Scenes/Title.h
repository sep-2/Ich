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
  // タイトルロゴ用フォント
  Font title_font_;

  // サブタイトル用フォント
  Font subtitle_font_;

  // プレスキー用フォント
  Font press_key_font_;

  // アニメーション用タイマー
  Stopwatch stopwatch_;

  // 背景グラデーション用テクスチャ
  Texture background_texture_;
};
