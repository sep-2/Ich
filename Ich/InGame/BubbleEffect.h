#pragma once
#include <Siv3D.hpp>

/// <summary>
/// 泡エフェクト（ゲームオーバー演出用）
/// </summary>
class BubbleEffect
{
public:
  BubbleEffect();
  ~BubbleEffect();

  /// <summary>
  /// 泡エフェクトを追加
  /// </summary>
  /// <param name="position">発生位置</param>
  void AddEffect(const Vec2& position);

  /// <summary>
  /// 更新処理
  /// </summary>
  /// <param name="delta_time">デルタタイム</param>
  void Update(double delta_time);

  /// <summary>
  /// 描画処理
  /// </summary>
  void Draw() const;

  /// <summary>
  /// すべてのエフェクトをクリア
  /// </summary>
  void Clear();

  /// <summary>
  /// エフェクトが存在するか
  /// </summary>
  bool HasEffects() const;

private:
  /// <summary>
  /// 泡パーティクル
  /// </summary>
  struct Bubble
  {
    Vec2 position;       // 位置
    Vec2 velocity;       // 速度
    double radius;       // 半径
    double lifetime;     // 経過時間
    double maxLifetime;  // 最大寿命
    ColorF color;        // 色
  };

  /// <summary>
  /// 泡エフェクト
  /// </summary>
  struct Effect
  {
    Array<Bubble> bubbles;  // 泡のリスト
    double elapsed;         // 経過時間
  };

  Array<Effect> effects_;
};
