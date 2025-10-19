#include "stdafx.h"

#include "Title.h"

#include "System/Renderer/Renderer.h"
#include "System/Audio/AudioManager.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/GameSettings.h"

namespace TitleConstants {
  const String TitleBgImageFilePath = U"Assets/Image/Title.png";
  const String kAnimalsImageFilePath = U"Assets/Image/Animals.png";
}

// コンストラクタ（必ず実装する）
Title::Title(const InitData& init)
  : IScene{ init }
{
  PRINT << U"Title::Title()";

  title_texture_ = std::make_shared<Texture>(TitleConstants::TitleBgImageFilePath);
  title_wrapper_ = std::make_shared<TextureWrapper>(title_texture_, 200, 0);

  animals_texture_ = std::make_shared<Texture>(TitleConstants::kAnimalsImageFilePath);
  animals_ = std::make_shared<TextureWrapper>(animals_texture_, 200, 0);

  // セーブデータが見つかればそれを読み込む
  {
    // バイナリファイルをオープン
    Deserializer<BinaryReader> reader{ U"game.save" };

    if (reader) // もしオープンに成功したら
    {
      SaveData saveData;

      reader(saveData);
      auto& data = getData<SaveData>();

      auto click_count = saveData.click_count_;
      data = saveData;
    }
  }
}

Title::~Title()
{
  PRINT << U"Title::~Title()";
}

// 更新関数
void Title::update()
{
  if (not m_stopwatch_.isStarted()) {
    m_stopwatch_.start();
  }

  // 左クリックで
  if (MouseL.down()) {
    m_stopwatch_.pause();

    // ゲームシーンに遷移
    changeScene(EnumScene::kInGame);
    AudioManager::GetInstance()->PlaySe(SeKind::kChangeSceneSe);
  }
}

// 描画関数
void Title::draw() const
{

  Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

  Renderer* renderer = Renderer::GetInstance();
  renderer->Push(Priority::kTitleBgPriority, std::static_pointer_cast<Task>(title_wrapper_));

  const double t = m_stopwatch_.sF();
  const Vec2 pos{ (400 + Periodic::Sine1_1(3s, t) * 300), 400 };
  animals_->SetPosition(static_cast<int>(pos.x), static_cast<int>(pos.y));
  animals_->SetIsCenter(true);
  animals_->SetScale(2.f);
  renderer->Push(Priority::kTitleBgPriority - 1, std::static_pointer_cast<Task>(animals_));

  Circle{ Cursor::Pos(), 50 }.draw(Palette::Seagreen);

  // 明るさ設定を適用
  GameSettings::GetInstance()->ApplyBrightness();
}

void Title::drawFadeIn(double t) const
{

  draw();

  Circle{ 400, 300, 600 }
  .drawFrame(((1 - t) * 600), 0, ColorF{ 0.2, 0.3, 0.4 });

}

void Title::drawFadeOut(double t) const
{
  draw();

  for (int32 y = 0; y < 6; ++y) {
    RectF{ (800 + y * 100 - t * 1600), (y * 100), 1600, 100 }.draw(HSV{ (y * 20), 0.2, 1.0 });
  }
}
