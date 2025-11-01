#pragma once
#include <Siv3D.hpp>

/// <summary>
/// 波背景エフェクトクラス（夢の世界の雰囲気を演出）
/// </summary>
class WaveBackgroundEffect
{
public:
  WaveBackgroundEffect();
  ~WaveBackgroundEffect();

  /// <summary>
  /// 更新処理
  /// </summary>
  /// <param name="deltaTime">デルタタイム</param>
  void Update(double deltaTime);

  /// <summary>
  /// 描画処理（背景画像の上、ゲーム要素の下に描画）
  /// </summary>
  /// <param name="cameraOffsetY">カメラのY座標オフセット（縦スクロール用）</param>
  void Draw(double cameraOffsetY) const;

private:
  /// <summary>
  /// 波の情報
  /// </summary>
  struct Wave
  {
    double amplitude;      // 振幅
    double frequency;      // 周波数
    double speed;          // 速度
    double phase;          // 位相
    ColorF color;          // 色（透過含む、アニメーション用）
    double yOffset;        // Y座標オフセット
    double colorPhase;     // 色アニメーションの位相
    ColorF baseColor;      // 基本色（アニメーション計算用）
    
    // sin/cosによる複雑な動きのパラメータ
    double sinScalar;      // sinのスカラー（ランダム）
    double cosScalar;      // cosのスカラー（ランダム）
    double sinFreq;        // sinの周波数（ランダム）
    double cosFreq;        // cosの周波数（ランダム）
    double movementPhase;  // 動きの位相
  };

  Array<Wave> waves_;      // 波の配列
  double elapsed_time_;    // 経過時間

  // 波のパラメータ定数
  static constexpr size_t kWaveCount = 5;               // 波の数
  static constexpr double kBaseAmplitude = 20.0;        // 基本振幅
  static constexpr double kBaseFrequency = 0.02;        // 基本周波数
  static constexpr double kBaseSpeed = 50.0;            // 基本速度
  static constexpr double kWaveHeight = 120.0;          // 波の高さ（間隔）
  static constexpr double kBaseAlpha = 0.15;            // 基本透明度
  static constexpr double kColorChangeSpeed = 0.3;      // 色変化の速度
  static constexpr double kMovementSpeed = 0.5;         // sin/cos動きの速度
};
