#include "stdafx.h"

#include "Title.h"

#include "System/Audio/AudioManager.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/GameSettings.h"

namespace TitleConstants
{
  // タイトル名
  const String kTitleText = U"ワードりらー";
  const String kSubtitleText = U"～文字を集めて単語を作ろう～";
  const String kPressKeyText = U"Enterでスタート";

  // フォントファイルパス
  const String kFontPath = U"Assets/Font/PottaOne-Regular.ttf";

  // フォントサイズ
  constexpr int32 kTitleFontSize = 120;
  constexpr int32 kSubtitleFontSize = 32;
  constexpr int32 kPressKeyFontSize = 40;

  // 色設定
  const ColorF kBackgroundColor1{ 0.2, 0.4, 0.8 };      // 青系グラデーション（上）
  const ColorF kBackgroundColor2{ 0.6, 0.3, 0.9 };      // 紫系グラデーション（下）
  const ColorF kTitleColor{ 1.0, 1.0, 0.3 };            // 黄色
  const ColorF kTitleShadowColor{ 0.3, 0.1, 0.0, 0.8 }; // 影
  const ColorF kSubtitleColor{ 1.0, 1.0, 1.0 };         // 白
  const ColorF kPressKeyColor{ 1.0, 1.0, 1.0 };         // 白

  // 位置設定
  const Vec2 kTitleShadowOffset{ 4.0, 4.0 };
  constexpr int32 kTitleY = 200;
  constexpr int32 kSubtitleY = 340;
  constexpr int32 kPressKeyY = 550;

  // アニメーション設定
  constexpr double kPressKeyBlinkInterval = 1.0;  // 点滅間隔（秒）
  constexpr double kTitlePulseInterval = 2.0;     // タイトルの脈動間隔（秒）
}

// コンストラクタ（必ず実装する）
Title::Title(const InitData& init)
  : IScene{ init }
  , title_font_(FontMethod::MSDF, TitleConstants::kTitleFontSize, TitleConstants::kFontPath, FontStyle::Bold)
  , subtitle_font_(FontMethod::MSDF, TitleConstants::kSubtitleFontSize, TitleConstants::kFontPath)
  , press_key_font_(FontMethod::MSDF, TitleConstants::kPressKeyFontSize, TitleConstants::kFontPath, FontStyle::Bold)
{
  PRINT << U"Title::Title()";

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

  // タイマー開始
  stopwatch_.start();

  // BGM再生（タイトル用のBGMがあれば）
  // AudioManager::GetInstance()->PlayBgm(BgmKind::kTitle);
}

Title::~Title()
{
  PRINT << U"Title::~Title()";
}

// 更新関数
void Title::update()
{
  // クリックまたはEnterキーでゲーム開始
  if (KeyEnter.down()) {
    stopwatch_.pause();

    // ゲームシーンに遷移
    changeScene(EnumScene::kInGame);
    AudioManager::GetInstance()->PlaySe(SeKind::kCompleteWord);
  }
}

// 描画関数
void Title::draw() const
{
  // 背景グラデーション
  const Rect background_rect{ 0, 0, Scene::Width(), Scene::Height() };
  background_rect.draw(Arg::top = TitleConstants::kBackgroundColor1, Arg::bottom = TitleConstants::kBackgroundColor2);

  // 経過時間を取得
  const double t = stopwatch_.sF();

  // タイトルテキスト（影付き、脈動アニメーション）
  const double title_pulse = 1.0 + 0.05 * Sin(t * Math::TwoPi / TitleConstants::kTitlePulseInterval);
  const auto title_glyph = title_font_(TitleConstants::kTitleText);
  const Vec2 title_center{ Scene::Center().x, TitleConstants::kTitleY };

  // タイトルの影
  title_glyph.drawAt(title_center + TitleConstants::kTitleShadowOffset, TitleConstants::kTitleShadowColor);

  // タイトル本体（脈動）
  {
    const Transformer2D transform{ Mat3x2::Scale(title_pulse, title_center) };
    title_glyph.drawAt(title_center, TitleConstants::kTitleColor);
  }

  // サブタイトル
  const auto subtitle_glyph = subtitle_font_(TitleConstants::kSubtitleText);
  const Vec2 subtitle_pos{ Scene::Center().x, TitleConstants::kSubtitleY };
  subtitle_glyph.drawAt(subtitle_pos, TitleConstants::kSubtitleColor);

  // 「クリックしてスタート」テキスト（点滅）
  const double blink_alpha = 0.3 + 0.7 * Periodic::Sine0_1(TitleConstants::kPressKeyBlinkInterval * 2.0, t);
  const auto press_key_glyph = press_key_font_(TitleConstants::kPressKeyText);
  const Vec2 press_key_pos{ Scene::Center().x, TitleConstants::kPressKeyY };
  press_key_glyph.drawAt(press_key_pos, ColorF{ TitleConstants::kPressKeyColor, blink_alpha });

  // 装飾的な要素（キラキラエフェクト）
  for (int i = 0; i < 20; ++i) {
    const double angle = (t * 0.5 + i * 0.5) * Math::TwoPi;
    const double radius = 300 + 50 * Sin(t * 2.0 + i);
    const Vec2 center_pos = Scene::Center();
    const Vec2 pos = center_pos + Circular{ radius, angle };
    const double size = 3.0 + 2.0 * Sin(t * 3.0 + i * 0.3);
    const double alpha = 0.3 + 0.3 * Sin(t * 4.0 + i * 0.7);
    Circle{ pos, size }.draw(ColorF{ 1.0, 1.0, 0.8, alpha });
  }

  // カーソル
  Circle{ Cursor::Pos(), 10 }.draw(ColorF{ 1.0, 0.8, 0.2, 0.5 });
  Circle{ Cursor::Pos(), 6 }.draw(ColorF{ 1.0, 1.0, 1.0 });

  // 明るさ設定を適用
  GameSettings::GetInstance()->ApplyBrightness();
}

void Title::drawFadeIn(double t) const
{
  draw();

  // フェードイン効果（円形）
  Circle{ Scene::Center(), 800 }
  .drawFrame(((1 - t) * 800), 0, ColorF{ 0.0, 0.0, 0.0 });
}

void Title::drawFadeOut(double t) const
{
  draw();

  // フェードアウト効果（横スライド）
  for (int32 y = 0; y < 8; ++y) {
    RectF{ (Scene::Width() + y * 100 - t * 2000), (y * (Scene::Height() / 8.0)), 2000, (Scene::Height() / 8.0) }
    .draw(HSV{ (y * 30), 0.5, 0.3 });
  }
}
