#include "stdafx.h"
#include "WaveBackgroundEffect.h"

WaveBackgroundEffect::WaveBackgroundEffect()
  : elapsed_time_(0.0)
{
  // 27色の暖色系グラデーションカラー（黄色→緑→水色）
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
    ColorF{ 0.7, 0.8, 1.0, kBaseAlpha }
  };
  
  // 複数の波を初期化（夢の世界の雰囲気）
  waves_.resize(kWaveCount);
  
  for (size_t i = 0; i < kWaveCount; ++i)
  {
    Wave& wave = waves_[i];
    
    // 各波に異なるパラメータを設定
    wave.amplitude = kBaseAmplitude * (1.0 + i * 0.3);
    wave.frequency = kBaseFrequency * (1.0 + i * 0.2);
    wave.speed = kBaseSpeed * (1.0 + i * 0.15) * ((i % 2 == 0) ? 1.0 : -1.0);  // 交互に方向を変える
    wave.phase = Math::TwoPi * i / kWaveCount;  // 位相をずらす
    wave.yOffset = kWaveHeight * i;
    
    // 27色からインデックスに応じて色を選択（暖色系）
    const size_t colorIndex = (i * 5) % gradientColors.size();  // 5つずつ飛ばして色を選択
    wave.color = gradientColors[colorIndex];
    
    // アニメーション用のパラメータ
    wave.colorPhase = Math::TwoPi * i / kWaveCount;  // 色アニメーションの位相
    wave.baseColor = gradientColors[colorIndex];      // 基本色を保存
    
    // sin/cos による複雑な動きのパラメータ（ランダム）
    wave.sinScalar = Random(0.5, 2.0);   // sinのスカラー（0.5～2.0）
    wave.cosScalar = Random(0.5, 2.0);   // cosのスカラー（0.5～2.0）
    wave.sinFreq = Random(0.3, 1.5);     // sinの周波数（0.3～1.5）
    wave.cosFreq = Random(0.3, 1.5);     // cosの周波数（0.3～1.5）
    wave.movementPhase = Random(0.0, Math::TwoPi);  // 動きの初期位相
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
    // 波の位相を更新（基本の動き）
    wave.phase += wave.speed * deltaTime * 0.001;  // ゆっくりと動く
    
    // 位相を0～2πの範囲に正規化
    while (wave.phase > Math::TwoPi)
    {
      wave.phase -= Math::TwoPi;
    }
    while (wave.phase < 0.0)
    {
      wave.phase += Math::TwoPi;
    }
    
    // sin/cosによる複雑な動きの位相を更新
    wave.movementPhase += deltaTime * kMovementSpeed;
    while (wave.movementPhase > Math::TwoPi)
    {
      wave.movementPhase -= Math::TwoPi;
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
    // 波の形状を作成（滑らかな曲線 + sin/cosによる複雑な動き）
    Array<Vec2> points;
    points.reserve(screenWidth + 1);
    
    // 波のパターンを無限に繰り返すために、カメラオフセットに基づいて開始位置を調整
    // 波の高さの2倍の周期で繰り返す
    const double wavePeriod = kWaveHeight * kWaveCount * 2.0;
    const double adjustedYOffset = wave.yOffset - Math::Fmod(cameraOffsetY, wavePeriod);
    
    // 画面外も含めて波を描画（上下に余裕を持たせる）
    const double extraRange = screenHeight * 0.5;
    
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
      
      // すべての動きを加算（カメラオフセットに基づいて調整）
      const double y = adjustedYOffset + baseY + sinMovement + cosMovement;
      
      points.emplace_back(x, y);
    }
    
    // 波の下側の頂点を追加（塗りつぶし用）
    Array<Vec2> fillPoints = points;
    fillPoints.emplace_back(screenWidth, screenHeight + extraRange);  // 右下（余裕を持たせる）
    fillPoints.emplace_back(0, screenHeight + extraRange);            // 左下（余裕を持たせる）
    
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
    // 無限スクロールのために、周期的に繰り返す
    const double additionalYOffset = adjustedYOffset + wavePeriod;
    
    // 追加の波が画面内に入る場合のみ描画
    if (additionalYOffset < screenHeight + extraRange)
    {
      Array<Vec2> additionalPoints;
      additionalPoints.reserve(screenWidth + 1);
      
      for (int32 x = 0; x <= screenWidth; x += kStep)
      {
        const double normalizedX = x * wave.frequency;
        const double baseY = wave.amplitude * Math::Sin(normalizedX + wave.phase);
        const double sinMovement = wave.sinScalar * Math::Sin(normalizedX * wave.sinFreq + wave.movementPhase);
        const double cosMovement = wave.cosScalar * Math::Cos(normalizedX * wave.cosFreq + wave.movementPhase * 0.7);
        const double y = additionalYOffset + baseY + sinMovement + cosMovement;
        
        additionalPoints.emplace_back(x, y);
      }
      
      Array<Vec2> additionalFillPoints = additionalPoints;
      additionalFillPoints.emplace_back(screenWidth, screenHeight + extraRange);
      additionalFillPoints.emplace_back(0, screenHeight + extraRange);
      
      Polygon{ additionalFillPoints }.draw(wave.color);
      
      for (size_t i = 0; i < additionalPoints.size() - 1; ++i)
      {
        Line{ additionalPoints[i], additionalPoints[i + 1] }.draw(2.0, lineColor);
      }
    }
    
    // 追加の波を描画（下方向にもう一セット）
    const double lowerYOffset = adjustedYOffset - wavePeriod;
    
    // 追加の波が画面内に入る場合のみ描画
    if (lowerYOffset > -extraRange)
    {
      Array<Vec2> lowerPoints;
      lowerPoints.reserve(screenWidth + 1);
      
      for (int32 x = 0; x <= screenWidth; x += kStep)
      {
        const double normalizedX = x * wave.frequency;
        const double baseY = wave.amplitude * Math::Sin(normalizedX + wave.phase);
        const double sinMovement = wave.sinScalar * Math::Sin(normalizedX * wave.sinFreq + wave.movementPhase);
        const double cosMovement = wave.cosScalar * Math::Cos(normalizedX * wave.cosFreq + wave.movementPhase * 0.7);
        const double y = lowerYOffset + baseY + sinMovement + cosMovement;
        
        lowerPoints.emplace_back(x, y);
      }
      
      Array<Vec2> lowerFillPoints = lowerPoints;
      lowerFillPoints.emplace_back(screenWidth, screenHeight + extraRange);
      lowerFillPoints.emplace_back(0, screenHeight + extraRange);
      
      Polygon{ lowerFillPoints }.draw(wave.color);
      
      for (size_t i = 0; i < lowerPoints.size() - 1; ++i)
      {
        Line{ lowerPoints[i], lowerPoints[i + 1] }.draw(2.0, lineColor);
      }
    }
  }
}
