#include "stdafx.h"
#include "MissionBanner.h"

MissionBanner::MissionBanner()
  : phase_(Phase::kIdle)
  , elapsed_time_(0.0)
  , word_count_(0)
  , font_(FontMethod::MSDF, 48, U"Assets/Font/Kosugi-Regular.ttf", FontStyle::Bold)
{
}

MissionBanner::~MissionBanner()
{
}

void MissionBanner::Start(int32 wordCount)
{
  phase_ = Phase::kBannerExpand;
  elapsed_time_ = 0.0;
  word_count_ = wordCount;
  mission_text_ = U"単語を{}つくれ！"_fmt(wordCount);
}

void MissionBanner::Update(double deltaTime)
{
  if (phase_ == Phase::kIdle || phase_ == Phase::kFinished) {
    return;
  }

  elapsed_time_ += deltaTime;

  switch (phase_) {
  case Phase::kBannerExpand:
    if (elapsed_time_ >= kBannerExpandDuration) {
      phase_ = Phase::kTextFadeIn;
      elapsed_time_ = 0.0;
    }
    break;

  case Phase::kTextFadeIn:
    if (elapsed_time_ >= kTextFadeInDuration) {
      phase_ = Phase::kTextDisplay;
      elapsed_time_ = 0.0;
    }
    break;

  case Phase::kTextDisplay:
    if (elapsed_time_ >= kTextDisplayDuration) {
      phase_ = Phase::kTextFadeOut;
      elapsed_time_ = 0.0;
    }
    break;

  case Phase::kTextFadeOut:
    if (elapsed_time_ >= kTextFadeOutDuration) {
      phase_ = Phase::kBannerShrink;
      elapsed_time_ = 0.0;
    }
    break;

  case Phase::kBannerShrink:
    if (elapsed_time_ >= kBannerShrinkDuration) {
      phase_ = Phase::kFinished;
      elapsed_time_ = 0.0;
    }
    break;

  default:
    break;
  }
}

void MissionBanner::Draw() const
{
  if (phase_ == Phase::kIdle || phase_ == Phase::kFinished) {
    return;
  }

  const Vec2 screen_center = Scene::Center();
  double banner_height = 0.0;
  double text_alpha = 0.0;
  double text_offset_x = 0.0;

  // フェーズに応じてバナーの高さとテキストの透明度を計算
  switch (phase_) {
  case Phase::kBannerExpand:
    // 黒い帯が広がる
    banner_height = EaseOutCubic(elapsed_time_ / kBannerExpandDuration) * kMaxBannerHeight;
    text_alpha = 0.0;
    break;

  case Phase::kTextFadeIn:
    // テキストがフェードイン（左から中央へ）
    banner_height = kMaxBannerHeight;
    text_alpha = EaseOutCubic(elapsed_time_ / kTextFadeInDuration);
    text_offset_x = -Scene::Width() * 0.5 * (1.0 - EaseOutCubic(elapsed_time_ / kTextFadeInDuration));
    break;

  case Phase::kTextDisplay:
    // テキスト表示中（静止）
    banner_height = kMaxBannerHeight;
    text_alpha = 1.0;
    text_offset_x = 0.0;
    break;

  case Phase::kTextFadeOut:
    // テキストがフェードアウト（中央から右へ）
    banner_height = kMaxBannerHeight;
    text_alpha = 1.0 - EaseInCubic(elapsed_time_ / kTextFadeOutDuration);
    text_offset_x = Scene::Width() * 0.5 * EaseInCubic(elapsed_time_ / kTextFadeOutDuration);
    break;

  case Phase::kBannerShrink:
    // 黒い帯が縮む
    banner_height = kMaxBannerHeight * (1.0 - EaseInCubic(elapsed_time_ / kBannerShrinkDuration));
    text_alpha = 0.0;
    break;

  default:
    break;
  }

  // 黒い半透明の帯を描画
  if (banner_height > 0.0) {
    const RectF banner_rect{
      0.0,
      screen_center.y - banner_height / 2.0,
      Scene::Width(),
      banner_height
    };
    banner_rect.draw(ColorF{ 0.0, 0.0, 0.0, 0.7 });
  }

  // テキストを描画
  if (text_alpha > 0.0) {
    const Vec2 text_pos = screen_center + Vec2{ text_offset_x, 0.0 };

    // 影を描画
    const Vec2 shadow_offset{ 3.0, 3.0 };
    font_(mission_text_).drawAt(text_pos + shadow_offset, ColorF{ 0.0, 0.0, 0.0, text_alpha * 0.5 });

    // テキスト本体を描画（金色）
    font_(mission_text_).drawAt(text_pos, ColorF{ 1.0, 0.9, 0.3, text_alpha });
  }
}

bool MissionBanner::IsFinished() const
{
  return phase_ == Phase::kFinished;
}
