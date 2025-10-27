#pragma once
#include <Siv3D.hpp>

// 獲得単語のヒット演出（ワールド座標で描画）
class HitEffect
{
  struct Entry
  {
    String text;
    Vec2 pos;          // ワールド座標（中心）
    double time = 0.0; // 経過秒
    double life = 1.2; // 寿命
    double rise = 80.0; // 上昇速度(px/s)
  };

  Array<Entry> effects_;
  Font font_;

public:
  HitEffect()
    : font_(80, Typeface::Bold) {}

  void Add(const Vec2& worldCenter, const String& text)
  {
    Entry e;
    e.text = text;
    e.pos = worldCenter;
    e.time = 0.0;
    e.life = 1.2;
    e.rise = 80.0;
    effects_ << e;
  }

  void Update(double dt)
  {
    // 更新と生存判定
    Array<Entry> alive;
    alive.reserve(effects_.size());
    for (auto& e : effects_)
    {
      e.time += dt;
      e.pos.y -= static_cast<float>(e.rise * dt);
      if (e.time < e.life)
      {
        alive << e;
      }
    }
    effects_.swap(alive);
  }

  void Draw() const
  {
    for (const auto& e : effects_)
    {
      const double t = (e.time / e.life);
      const double alpha = Clamp(1.0 - t, 0.0, 1.0) * 0.9; // 半透明からフェードアウト

      // 影
      const Vec2 shadowOffset{ 3.0, 3.0 };
      font_(e.text).drawAt(e.pos + shadowOffset, ColorF{ 0, 0, 0, alpha * 0.6 });
      // 本体
      font_(e.text).drawAt(e.pos, ColorF{ 1, 1, 1, alpha });
    }
  }
};
