#pragma once
#include <Siv3D.hpp>

/// <summary>
/// ブロック破壊演出クラス
/// </summary>
class BlockDestroyEffect
{
public:
  /// <summary>
  /// パーティクル情報
  /// </summary>
  struct Particle
  {
    Vec2 position;
    Vec2 velocity;
    ColorF color;
    double size;
    double lifetime;
    double maxLifetime;
  };

  /// <summary>
  /// 破壊演出情報
  /// </summary>
  struct DestroyEffect
  {
    Vec2 position;
    Array<Particle> particles;
    double elapsed;
    ColorF blockColor;
    String blockText;
  };

  BlockDestroyEffect();
  ~BlockDestroyEffect();

  /// <summary>
  /// 破壊演出を追加
  /// </summary>
  /// <param name="position">ブロックの中心位置</param>
  /// <param name="color">ブロックの色</param>
  /// <param name="text">ブロックのテキスト</param>
  void AddEffect(const Vec2& position, const ColorF& color, const String& text);

  /// <summary>
  /// 更新処理
  /// </summary>
  /// <param name="deltaTime">デルタタイム</param>
  void Update(double deltaTime);

  /// <summary>
  /// 描画処理
  /// </summary>
  void Draw() const;

  /// <summary>
  /// すべての演出をクリア
  /// </summary>
  void Clear();

private:
  Array<DestroyEffect> effects_;
  Font font_;
};
