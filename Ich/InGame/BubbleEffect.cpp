#include "stdafx.h"
#include "BubbleEffect.h"

namespace BubbleEffectConstants
{
  // 泡の設定
  constexpr int32 kBubbleCount = 30;             // 泡の数
  constexpr double kBubbleMinRadius = 5.0;       // 最小半径
  constexpr double kBubbleMaxRadius = 20.0;      // 最大半径
  constexpr double kBubbleMinSpeed = 20.0;       // 最小速度
  constexpr double kBubbleMaxSpeed = 80.0;       // 最大速度
  constexpr double kBubbleLifetime = 2.5;        // 泡の寿命（秒）
  constexpr double kBubbleUpwardBias = -120.0;   // 上方向バイアス
  constexpr double kEffectDuration = 3.0;        // 演出全体の持続時間（秒）
  
  // 泡の色
  const ColorF kBubbleColor{ 0.8, 0.9, 1.0 };    // 薄い青色
  const ColorF kBubbleInnerColor{ 1.0, 1.0, 1.0 }; // 内側の色
  
  // 物理パラメータ
  constexpr double kBubbleFloatAccel = -50.0;    // 浮力加速度
  constexpr double kBubbleDrag = 0.95;           // 抵抗係数
  
  // 描画設定
  constexpr double kBubbleFrameThickness = 2.0;  // 泡の枠線の太さ
  constexpr double kBubbleInnerRadiusRatio = 0.4; // 内側のハイライト半径比
}

BubbleEffect::BubbleEffect()
{
}

BubbleEffect::~BubbleEffect()
{
}

void BubbleEffect::AddEffect(const Vec2& position)
{
  Effect effect;
  effect.elapsed = 0.0;

  // 泡を生成
  for (int32 i = 0; i < BubbleEffectConstants::kBubbleCount; ++i)
  {
    Bubble bubble;
    bubble.position = position;

    // ランダムな方向と速度
    const double angle = Random(Math::TwoPi);
    const double speed = Random(BubbleEffectConstants::kBubbleMinSpeed, BubbleEffectConstants::kBubbleMaxSpeed);
    bubble.velocity = Vec2{
      Math::Cos(angle) * speed,
      Math::Sin(angle) * speed + BubbleEffectConstants::kBubbleUpwardBias
    };

    // サイズと色
    bubble.radius = Random(BubbleEffectConstants::kBubbleMinRadius, BubbleEffectConstants::kBubbleMaxRadius);
    bubble.color = BubbleEffectConstants::kBubbleColor;
    bubble.lifetime = 0.0;
    bubble.maxLifetime = BubbleEffectConstants::kBubbleLifetime;

    effect.bubbles << bubble;
  }

  effects_ << effect;
}

void BubbleEffect::Update(double delta_time)
{
  // 各演出を更新
  for (auto it = effects_.begin(); it != effects_.end();)
  {
    it->elapsed += delta_time;

    // 演出時間が終了したら削除
    if (it->elapsed >= BubbleEffectConstants::kEffectDuration)
    {
      it = effects_.erase(it);
      continue;
    }

    // 泡を更新
    for (auto& bubble : it->bubbles)
    {
      bubble.lifetime += delta_time;

      // 浮力を適用
      bubble.velocity.y += BubbleEffectConstants::kBubbleFloatAccel * delta_time;

      // 抵抗を適用
      bubble.velocity *= BubbleEffectConstants::kBubbleDrag;

      // 位置を更新
      bubble.position += bubble.velocity * delta_time;

      // 横揺れ効果
      bubble.position.x += Math::Sin(bubble.lifetime * 3.0) * 2.0 * delta_time;
    }

    ++it;
  }
}

void BubbleEffect::Draw() const
{
  for (const auto& effect : effects_)
  {
    // 泡を描画
    for (const auto& bubble : effect.bubbles)
    {
      // フェードアウト
      const double alpha = Max(0.0, 1.0 - (bubble.lifetime / bubble.maxLifetime));
      if (alpha > 0.0)
      {
        ColorF draw_color = bubble.color;
        draw_color.a = alpha * 0.7;

        // 泡の本体
        Circle{ bubble.position, bubble.radius }.draw(draw_color);

        // 泡の枠線
        ColorF frame_color = bubble.color;
        frame_color.a = alpha * 0.9;
        Circle{ bubble.position, bubble.radius }.drawFrame(BubbleEffectConstants::kBubbleFrameThickness, frame_color);

        // ハイライト（泡の光沢）
        const Vec2 highlight_offset{ -bubble.radius * 0.3, -bubble.radius * 0.3 };
        ColorF highlight_color = BubbleEffectConstants::kBubbleInnerColor;
        highlight_color.a = alpha * 0.5;
        Circle{ bubble.position + highlight_offset, bubble.radius * BubbleEffectConstants::kBubbleInnerRadiusRatio }.draw(highlight_color);
      }
    }
  }
}

void BubbleEffect::Clear()
{
  effects_.clear();
}

bool BubbleEffect::HasEffects() const
{
  return !effects_.isEmpty();
}
