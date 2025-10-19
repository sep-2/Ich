#pragma once
#include "Scenes/Enum.h"
#include "System/Renderer/TextureWrapper.h"
#include "System/Renderer/Renderer.h"
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

  std::shared_ptr<Texture> title_texture_;

  /// <summary>
  /// イメージ
  /// </summary>
  std::shared_ptr<TextureWrapper> title_wrapper_;

  std::shared_ptr<Texture> animals_texture_;

  std::shared_ptr<TextureWrapper> animals_;

  Stopwatch m_stopwatch_;
};
