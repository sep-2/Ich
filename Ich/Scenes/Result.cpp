#include "stdafx.h"

#include "Result.h"

namespace ResultConstants {
  const String kGameClearImagePath = U"Assets/Image/Player/GameClear.png";
  const String kGameOverImagePath = U"Assets/Image/Player/GameOver2.png";
  constexpr double kImageMaxWidth = 900.0;
  constexpr double kImageCenterY = 240.0;
  constexpr double kListAreaMargin = 100.0;
  constexpr double kListTop = 360.0;
  constexpr double kColumnWidth = 320.0;
  constexpr double kLineSpacing = 8.0;
  constexpr int32 kColumnCount = 3;
}

Result::Result(const InitData& init)
  : SceneManager<EnumScene, SaveData>::Scene{ init }
  , shared_data_(getData<SaveData>())
  , game_clear_texture_(ResultConstants::kGameClearImagePath)
  , game_over_texture_(ResultConstants::kGameOverImagePath)
  , title_font_{ 56, Typeface::Bold }
  , word_font_{ 28 }
  , instruction_font_{ 24 }
{
  if (game_clear_texture_.isEmpty()) {
    PRINT << U"[Result] 画像の読み込みに失敗しました: " << ResultConstants::kGameClearImagePath;
  }

  if (game_over_texture_.isEmpty()) {
    PRINT << U"[Result] 画像の読み込みに失敗しました: " << ResultConstants::kGameOverImagePath;
  }
}

void Result::update()
{
  if (IsAnyKeyTriggered()) {
    changeScene(EnumScene::kTitle, 0s);
  }
}

void Result::draw() const
{
  Scene::SetBackground(ColorF{ 0.05, 0.07, 0.12 });

  const bool cleared = shared_data_.last_game_cleared_;
  const Texture& banner = cleared ? game_clear_texture_ : game_over_texture_;
  if (!banner.isEmpty()) {
    const double scale = Min(ResultConstants::kImageMaxWidth / banner.width(), 1.0);
    banner.scaled(scale * (3.0 / 5.0)).drawAt(Scene::Center().x, ResultConstants::kImageCenterY);
  }

  const String title = cleared ? U"ゲームクリア！" : U"ゲームオーバー";
  const Vec2 title_shadow_offset{ 2.0, 2.0 };
  title_font_(title).drawAt(Scene::Center().x + title_shadow_offset.x, 120 + title_shadow_offset.y, ColorF{ 0.0, 0.0, 0.0, 0.45 });
  title_font_(title).drawAt(Scene::Center().x, 120, Palette::White);

  DrawWordList();

  instruction_font_(U"何かキーを押すとタイトルへ戻ります").drawAt(Scene::Center().x, Scene::Height() - 60, ColorF{ 1.0 });
}

void Result::DrawWordList() const
{
  const RectF list_area{ ResultConstants::kListAreaMargin, ResultConstants::kListTop, Scene::Width() - ResultConstants::kListAreaMargin * 2.0, Scene::Height() - ResultConstants::kListTop - 120.0 };
  list_area.draw(ColorF{ 0.0, 0.0, 0.0, 0.45 });
  list_area.drawFrame(2.0, ColorF{ 1.0, 1.0, 1.0, 0.35 });

  const auto& words = shared_data_.last_completed_words_;

  // 背景に単語数を大きく薄く表示してアクセントを付ける
  const String overlay_text = U"{}"_fmt(words.size());
  if (!overlay_text.isEmpty()) {
    const Vec2 overlay_center = list_area.center();
    const double overlay_scale = 3.6;
    const Transformer2D overlay_transform{ Mat3x2::Scale(overlay_scale, overlay_center) };
    title_font_(overlay_text).drawAt(overlay_center, ColorF{ 1.0, 1.0, 1.0, 0.12 });
  }

  if (words.isEmpty()) {
    word_font_(U"完成した単語はありません").drawAt(list_area.center(), Palette::White);
    return;
  }

  int32 columns = ResultConstants::kColumnCount;
  if (words.size() < static_cast<size_t>(columns)) {
    columns = static_cast<int32>(words.size());
  }
  if (columns <= 0) {
    columns = 1;
  }

  const double line_height = word_font_.height() + ResultConstants::kLineSpacing;
  for (size_t i = 0; i < words.size(); ++i) {
    const size_t column = static_cast<size_t>(i % columns);
    const size_t row = static_cast<size_t>(i / columns);

    const double x = list_area.x + 20.0 + column * ResultConstants::kColumnWidth;
    const double y = list_area.y + 20.0 + row * line_height;

    word_font_(U"{:>2}. {}"_fmt(i + 1, words[i])).draw(x, y, Palette::White);
  }
}
bool Result::IsAnyKeyTriggered() const
{
  return KeyEnter.down()
    || KeySpace.down()
    || KeyEscape.down()
    || KeyZ.down() || KeyX.down()
    || KeyA.down() || KeyS.down() || KeyD.down() || KeyW.down()
    || KeyUp.down() || KeyDown.down() || KeyLeft.down() || KeyRight.down();
}
