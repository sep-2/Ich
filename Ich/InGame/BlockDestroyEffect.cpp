#include "stdafx.h"
#include "BlockDestroyEffect.h"

namespace BlockDestroyEffectConstants {
  // パーティクルの設定
  constexpr int32 kParticleCount = 20;           // パーティクル数
  constexpr double kParticleMinSpeed = 50.0;     // 最小速度
  constexpr double kParticleMaxSpeed = 200.0;    // 最大速度
  constexpr double kParticleMinSize = 3.0;       // 最小サイズ
  constexpr double kParticleMaxSize = 8.0;       // 最大サイズ
  constexpr double kParticleLifetime = 1.0;      // パーティクル寿命（秒）
  constexpr double kGravity = 300.0;             // 重力加速度
  constexpr double kEffectDuration = 1.5;        // 演出全体の持続時間（秒）
  constexpr double kParticleUpwardBias = -100.0; // パーティクルの上方向バイアス

  // テキスト演出の設定
  constexpr double kTextRiseSpeed = -50.0;       // テキスト上昇速度
  constexpr int32 kFontSize = 30;                // フォントサイズ

  // 影の設定
  const Vec2 kTextShadowOffset{ 2.0, 2.0 };      // テキスト影のオフセット
  constexpr double kTextShadowAlphaMultiplier = 0.5; // 影のアルファ乗数
  const ColorF kTextShadowColor{ 0.0, 0.0, 0.0 }; // 影の基本色

  // 爆発エフェクトの設定
  constexpr double kExplosionDuration = 0.3;     // 爆発演出の持続時間（秒）
  constexpr double kExplosionMaxRadius = 50.0;   // 爆発の最大半径
  constexpr double kExplosionFrameThickness = 3.0; // 爆発円の線の太さ
  constexpr double kExplosionAlphaMultiplier = 0.8; // 爆発のアルファ乗数
  const ColorF kExplosionColor{ 1.0, 1.0, 1.0 }; // 爆発の色
}

BlockDestroyEffect::BlockDestroyEffect()
  : font_(BlockDestroyEffectConstants::kFontSize, Typeface::Bold)
{
}

BlockDestroyEffect::~BlockDestroyEffect()
{
}

void BlockDestroyEffect::AddEffect(const Vec2& position, const ColorF& color, const String& text)
{
  DestroyEffect effect;
  effect.position = position;
  effect.elapsed = 0.0;
  effect.blockColor = color;
  effect.blockText = text;

  // パーティクルを生成
  for (int32 i = 0; i < BlockDestroyEffectConstants::kParticleCount; ++i) {
    Particle particle;
    particle.position = position;

    // ランダムな方向と速度
    const double angle = Random(Math::TwoPi);
    const double speed = Random(BlockDestroyEffectConstants::kParticleMinSpeed, BlockDestroyEffectConstants::kParticleMaxSpeed);
    particle.velocity = Vec2{
      Math::Cos(angle) * speed,
      Math::Sin(angle) * speed + BlockDestroyEffectConstants::kParticleUpwardBias  // 上方向にバイアス
    };

    // サイズと色
    particle.size = Random(BlockDestroyEffectConstants::kParticleMinSize, BlockDestroyEffectConstants::kParticleMaxSize);
    particle.color = color;
    particle.lifetime = 0.0;
    particle.maxLifetime = BlockDestroyEffectConstants::kParticleLifetime;

    effect.particles << particle;
  }

  effects_ << effect;
}

void BlockDestroyEffect::Update(double delta_time)
{
  // 各演出を更新
  for (auto it = effects_.begin(); it != effects_.end();) {
    it->elapsed += delta_time;

    // 演出時間が終了したら削除
    if (it->elapsed >= BlockDestroyEffectConstants::kEffectDuration) {
      it = effects_.erase(it);
      continue;
    }

    // パーティクルを更新
    for (auto& particle : it->particles) {
      particle.lifetime += delta_time;

      // 重力を適用
      particle.velocity.y += BlockDestroyEffectConstants::kGravity * delta_time;

      // 位置を更新
      particle.position += particle.velocity * delta_time;
    }

    ++it;
  }
}

void BlockDestroyEffect::Draw() const
{
  for (const auto& effect : effects_) {
    // パーティクルを描画
    for (const auto& particle : effect.particles) {
      // フェードアウト
      const double alpha = 1.0 - (particle.lifetime / particle.maxLifetime);
      if (alpha > 0.0) {
        ColorF draw_color = particle.color;
        draw_color.a = alpha;
        Circle{ particle.position, particle.size }.draw(draw_color);
      }
    }

    // テキストをフェードアウトしながら上昇
    const double text_alpha = Max(0.0, 1.0 - (effect.elapsed / BlockDestroyEffectConstants::kEffectDuration));
    if (text_alpha > 0.0) {
      const Vec2 text_pos = effect.position + Vec2{ 0, BlockDestroyEffectConstants::kTextRiseSpeed * effect.elapsed };
      ColorF text_color = effect.blockColor;
      text_color.a = text_alpha;

      // 影
      ColorF shadow_color = BlockDestroyEffectConstants::kTextShadowColor;
      shadow_color.a = text_alpha * BlockDestroyEffectConstants::kTextShadowAlphaMultiplier;
      font_(effect.blockText).drawAt(text_pos + BlockDestroyEffectConstants::kTextShadowOffset, shadow_color);
      // 本体
      font_(effect.blockText).drawAt(text_pos, text_color);
    }

    // 爆発の円
    if (effect.elapsed < BlockDestroyEffectConstants::kExplosionDuration) {
      const double explosion_alpha = 1.0 - (effect.elapsed / BlockDestroyEffectConstants::kExplosionDuration);
      const double explosion_radius = BlockDestroyEffectConstants::kExplosionMaxRadius * (effect.elapsed / BlockDestroyEffectConstants::kExplosionDuration);
      ColorF explosion_color = BlockDestroyEffectConstants::kExplosionColor;
      explosion_color.a = explosion_alpha * BlockDestroyEffectConstants::kExplosionAlphaMultiplier;
      Circle{ effect.position, explosion_radius }.drawFrame(BlockDestroyEffectConstants::kExplosionFrameThickness, explosion_color);
    }
  }
}

void BlockDestroyEffect::Clear()
{
  effects_.clear();
}
