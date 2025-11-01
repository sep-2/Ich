#include "stdafx.h"
#include "WaveBackgroundEffect.h"

WaveBackgroundEffect::WaveBackgroundEffect()
  : elapsed_time_(0.0)
{
  // 33色のグラデーションカラー（黄色→緑→水色→赤系を追加）
  const Array<ColorF> gradientColors = {
    // 濃い黄色～明るい黄色（1-5）
    ColorF{ 0.9, 0.8, 0.1, kBaseAlpha },
    ColorF{ 0.95, 0.85, 0.2, kBaseAlpha },
    ColorF{ 1.0, 0.9, 0.3, kBaseAlpha },
    ColorF{ 1.0, 0.95, 0.4, kBaseAlpha },
    ColorF{ 1.0, 1.0, 0.5, kBaseAlpha },
    
    // 黄緑（6-10）
    ColorF{ 0.95, 1.0, 0.4, kBaseAlpha },
    ColorF{ 0.9, 1.0, 0.4, kBaseAlpha },
    ColorF{ 0.85, 1.0, 0.3, kBaseAlpha },
    ColorF{ 0.8, 1.0, 0.3, kBaseAlpha },
    ColorF{ 0.75, 1.0, 0.35, kBaseAlpha },
    
    // 緑（11-15）
    ColorF{ 0.7, 1.0, 0.4, kBaseAlpha },
    ColorF{ 0.6, 1.0, 0.4, kBaseAlpha },
    ColorF{ 0.5, 1.0, 0.5, kBaseAlpha },
    ColorF{ 0.4, 1.0, 0.5, kBaseAlpha },
    ColorF{ 0.3, 1.0, 0.6, kBaseAlpha },
    
    // 緑～青緑（16-20）
    ColorF{ 0.3, 1.0, 0.7, kBaseAlpha },
    ColorF{ 0.3, 1.0, 0.8, kBaseAlpha },
    ColorF{ 0.3, 0.95, 0.9, kBaseAlpha },
    ColorF{ 0.3, 0.9, 1.0, kBaseAlpha },
    ColorF{ 0.3, 0.85, 1.0, kBaseAlpha },
    
    // 水色（21-25）
    ColorF{ 0.4, 0.8, 1.0, kBaseAlpha },
    ColorF{ 0.4, 0.75, 1.0, kBaseAlpha },
    ColorF{ 0.5, 0.7, 1.0, kBaseAlpha },
    ColorF{ 0.5, 0.65, 1.0, kBaseAlpha },
    ColorF{ 0.6, 0.7, 1.0, kBaseAlpha },
    
    // 明るい水色（26-27）
    ColorF{ 0.6, 0.75, 1.0, kBaseAlpha },
    ColorF{ 0.7, 0.8, 1.0, kBaseAlpha },
    
    // 赤系（28-33）追加
    ColorF{ 1.0, 0.3, 0.3, kBaseAlpha },  // 鮮やかな赤
    ColorF{ 1.0, 0.4, 0.4, kBaseAlpha },  // 明るい赤
    ColorF{ 1.0, 0.5, 0.5, kBaseAlpha },  // ピンクがかった赤
    ColorF{ 1.0, 0.3, 0.5, kBaseAlpha },  // ローズ
    ColorF{ 1.0, 0.4, 0.6, kBaseAlpha },  // ライトローズ
    ColorF{ 1.0, 0.5, 0.4, kBaseAlpha }   // サーモンピンク
  };
  
  // 複数の波を初期化（夢の世界の雰囲気）
  waves_.resize(kWaveCount);
  
  for (size_t i = 0; i < kWaveCount; ++i)
  {
    Wave& wave = waves_[i];
    
    // 各波に異なるパラメータを設定（速度を1.5倍に上げる）
    wave.amplitude = kBaseAmplitude * (1.0 + i * 0.3);
    wave.frequency = kBaseFrequency * (1.0 + i * 0.2);
    wave.speed = kBaseSpeed * 1.5 * (1.0 + i * 0.15) * ((i % 2 == 0) ? 1.0 : -1.0);  // 速度を1.5倍に
    wave.phase = Math::TwoPi * i / kWaveCount;  // 位相をずらす
    wave.yOffset = kWaveHeight * i;
    
    // 33色からインデックスに応じて色を選択（暖色系）
    const size_t colorIndex = (i * 5) % gradientColors.size();  // 5つずつ飛ばして色を選択
    wave.color = gradientColors[colorIndex];
    
    // アニメーション用のパラメータ
    wave.colorPhase = Math::TwoPi * i / kWaveCount;  // 色アニメーションの位相
    wave.baseColor = gradientColors[colorIndex];      // 基本色を保存
    
    // sin/cos による複雑な動きのパラメータ（ランダム、速度を1.5倍に）
    wave.sinScalar = Random(0.5, 2.0);   // sinのスカラー（0.5～2.0）
    wave.cosScalar = Random(0.5, 2.0);   // cosのスカラー（0.5～2.0）
    wave.sinFreq = Random(0.3, 1.5) * 1.5;     // sinの周波数（0.3～1.5の1.5倍）
    wave.cosFreq = Random(0.3, 1.5) * 1.5;     // cosの周波数（0.3～1.5の1.5倍）
    wave.movementPhase = Random(0.0, Math::TwoPi);  // 動きの初期位相
    
    // 回転パラメータ（0度に固定）
    wave.rotation = 0.0;  // 回転なし
    wave.rotationSpeed = 0.0;  // 回転速度もゼロ
  }
}

WaveBackgroundEffect::~WaveBackgroundEffect()
{
}

void WaveBackgroundEffect::Update(double deltaTime)
{
  elapsed_time_ += deltaTime;
  
  // 各波の位相と色を更新
  for (auto& wave : waves_)
  {
    // 波の位相を更新（基本の動き、速度を1.5倍に）
    wave.phase += wave.speed * deltaTime * 0.01;  // 0.001 から 0.0015 へ（1.5倍速）
    
    // 位相を0～2πの範囲に正規化
    while (wave.phase > Math::TwoPi)
    {
      wave.phase -= Math::TwoPi;
    }
    while (wave.phase < 0.0)
    {
      wave.phase += Math::TwoPi;
    }
    
    // sin/cosによる複雑な動きの位相を更新（速度を1.5倍に）
    wave.movementPhase += deltaTime * kMovementSpeed * 1.5;  // 1.5倍速
    while (wave.movementPhase > Math::TwoPi)
    {
      wave.movementPhase -= Math::TwoPi;
    }
    
    // 回転を更新
    wave.rotation += wave.rotationSpeed * deltaTime;
    while (wave.rotation > Math::TwoPi)
    {
      wave.rotation -= Math::TwoPi;
    }
    while (wave.rotation < -Math::TwoPi)
    {
      wave.rotation += Math::TwoPi;
    }
    
    // 色アニメーションの位相を更新（ゆっくりと色が変化）
    wave.colorPhase += deltaTime * kColorChangeSpeed;
    while (wave.colorPhase > Math::TwoPi)
    {
      wave.colorPhase -= Math::TwoPi;
    }
    
    // 色をアニメーション（基本色から明るさを変化させる）
    const double brightness = 0.8 + 0.2 * Math::Sin(wave.colorPhase);  // 0.8～1.0の範囲で明るさが変化
    wave.color = ColorF{
      wave.baseColor.r * brightness,
      wave.baseColor.g * brightness,
      wave.baseColor.b * brightness,
      wave.baseColor.a
    };
  }
}

void WaveBackgroundEffect::Draw(double cameraOffsetY) const
{
  // 画面サイズを取得
  const int32 screenWidth = Scene::Width();
  const int32 screenHeight = Scene::Height();
  
  // 各波を描画（背景として、加算合成で重ねる）
  const ScopedRenderStates2D blend{ BlendState::Additive };
  
  for (const auto& wave : waves_)
  {
    // 波のパターンを無限に繰り返すために、カメラオフセットに基づいて開始位置を調整
    // 波の高さの2倍の周期で繰り返す
    const double wavePeriod = kWaveHeight * kWaveCount * 2.0;
    const double adjustedYOffset = wave.yOffset - Math::Fmod(cameraOffsetY, wavePeriod);
    
    // 画面外も含めて波を描画（プレイヤーの位置から±1000ピクセルまで）
    constexpr double extraRange = 1000.0;
    
    // 回転の中心点（画面中央）
    const Vec2 rotationCenter{ screenWidth / 2.0, adjustedYOffset };
    
    // 回転用のヘルパー関数（ラムダ）
    auto rotatePoint = [](const Vec2& point, const Vec2& center, double angle) -> Vec2 {
      const Vec2 translated = point - center;
      const double cos_a = Math::Cos(angle);
      const double sin_a = Math::Sin(angle);
      const Vec2 rotated{
        translated.x * cos_a - translated.y * sin_a,
        translated.x * sin_a + translated.y * cos_a
      };
      return rotated + center;
    };
    
    // 波の形状を作成（滑らかな曲線 + sin/cosによる複雑な動き + 回転）
    Array<Vec2> points;
    points.reserve(screenWidth + 1);
    
    // 画面幅全体にわたって波を生成
    constexpr int32 kStep = 4;  // 描画の細かさ（小さいほど滑らか）
    for (int32 x = 0; x <= screenWidth; x += kStep)
    {
      // 基本のサイン波
      const double normalizedX = x * wave.frequency;
      const double baseY = wave.amplitude * Math::Sin(normalizedX + wave.phase);
      
      // sin/cosによる複雑な動き（スカラーとランダムな周波数）
      const double sinMovement = wave.sinScalar * Math::Sin(normalizedX * wave.sinFreq + wave.movementPhase);
      const double cosMovement = wave.cosScalar * Math::Cos(normalizedX * wave.cosFreq + wave.movementPhase * 0.7);
      
      // すべての動きを加算（回転前の座標）
      Vec2 point{ static_cast<double>(x), adjustedYOffset + baseY + sinMovement + cosMovement };
      
      // 回転を適用
      const Vec2 rotated = rotatePoint(point, rotationCenter, wave.rotation);
      
      points.emplace_back(rotated);
    }
    
    // 波の下側の頂点を追加（塗りつぶし用）
    Array<Vec2> fillPoints = points;
    
    // 下側の2点も回転を適用
    const Vec2 bottomRight{ static_cast<double>(screenWidth), screenHeight + extraRange };
    const Vec2 bottomLeft{ 0.0, screenHeight + extraRange };
    fillPoints.emplace_back(rotatePoint(bottomRight, rotationCenter, wave.rotation));
    fillPoints.emplace_back(rotatePoint(bottomLeft, rotationCenter, wave.rotation));
    
    // 波の塗りつぶし（透過、アニメーション色）
    Polygon{ fillPoints }.draw(wave.color);
    
    // 波の輪郭線（より明るく、少し太く）
    ColorF lineColor = wave.color;
    lineColor.a *= 2.0;  // 輪郭は少し濃く
    for (size_t i = 0; i < points.size() - 1; ++i)
    {
      Line{ points[i], points[i + 1] }.draw(2.0, lineColor);
    }
    
    // 追加の波を描画（上方向にもう一セット）
    const double additionalYOffset = adjustedYOffset + wavePeriod;
    
    if (additionalYOffset < screenHeight + extraRange)
    {
      const Vec2 additionalRotationCenter{ screenWidth / 2.0, additionalYOffset };
      Array<Vec2> additionalPoints;
      additionalPoints.reserve(screenWidth + 1);
      
      for (int32 x = 0; x <= screenWidth; x += kStep)
      {
        const double normalizedX = x * wave.frequency;
        const double baseY = wave.amplitude * Math::Sin(normalizedX + wave.phase);
        const double sinMovement = wave.sinScalar * Math::Sin(normalizedX * wave.sinFreq + wave.movementPhase);
        const double cosMovement = wave.cosScalar * Math::Cos(normalizedX * wave.cosFreq + wave.movementPhase * 0.7);
        
        Vec2 point{ static_cast<double>(x), additionalYOffset + baseY + sinMovement + cosMovement };
        const Vec2 rotated = rotatePoint(point, additionalRotationCenter, wave.rotation);
        
        additionalPoints.emplace_back(rotated);
      }
      
      Array<Vec2> additionalFillPoints = additionalPoints;
      additionalFillPoints.emplace_back(rotatePoint(bottomRight, additionalRotationCenter, wave.rotation));
      additionalFillPoints.emplace_back(rotatePoint(bottomLeft, additionalRotationCenter, wave.rotation));
      
      Polygon{ additionalFillPoints }.draw(wave.color);
      
      for (size_t i = 0; i < additionalPoints.size() - 1; ++i)
      {
        Line{ additionalPoints[i], additionalPoints[i + 1] }.draw(2.0, lineColor);
      }
    }
    
    // 追加の波を描画（下方向にもう一セット）
    const double lowerYOffset = adjustedYOffset - wavePeriod;
    
    if (lowerYOffset > -extraRange)
    {
      const Vec2 lowerRotationCenter{ screenWidth / 2.0, lowerYOffset };
      Array<Vec2> lowerPoints;
      lowerPoints.reserve(screenWidth + 1);
      
      for (int32 x = 0; x <= screenWidth; x += kStep)
      {
        const double normalizedX = x * wave.frequency;
        const double baseY = wave.amplitude * Math::Sin(normalizedX + wave.phase);
        const double sinMovement = wave.sinScalar * Math::Sin(normalizedX * wave.sinFreq + wave.movementPhase);
        const double cosMovement = wave.cosScalar * Math::Cos(normalizedX * wave.cosFreq + wave.movementPhase * 0.7);
        
        Vec2 point{ static_cast<double>(x), lowerYOffset + baseY + sinMovement + cosMovement };
        const Vec2 rotated = rotatePoint(point, lowerRotationCenter, wave.rotation);
        
        lowerPoints.emplace_back(rotated);
      }
      
      Array<Vec2> lowerFillPoints = lowerPoints;
      lowerFillPoints.emplace_back(rotatePoint(bottomRight, lowerRotationCenter, wave.rotation));
      lowerFillPoints.emplace_back(rotatePoint(bottomLeft, lowerRotationCenter, wave.rotation));
      
      Polygon{ lowerFillPoints }.draw(wave.color);
      
      for (size_t i = 0; i < lowerPoints.size() - 1; ++i)
      {
        Line{ lowerPoints[i], lowerPoints[i + 1] }.draw(2.0, lineColor);
      }
    }
  }
}
