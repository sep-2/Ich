#include "stdafx.h"

#include "Title.h"

#include "System/Audio/AudioManager.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/GameSettings.h"

namespace TitleConstants
{
  // タイトル名
  const String kTitleText = U"わードリーム";
  const String kSubtitleText = U"～文字を集めて単語を作ろう～";
  
  // メニュー項目
  const String kMenuGameStart = U"ゲーム開始";
  const String kMenuCredits = U"クレジット";
  const String kMenuExit = U"ゲーム終了";

  // フォントファイルパス
  const String kFontPath = U"Assets/Font/PottaOne-Regular.ttf";

  // フォントサイズ
  constexpr int32 kTitleFontSize = 120;
  constexpr int32 kSubtitleFontSize = 32;
  constexpr int32 kMenuFontSize = 40;

  // 色設定（時間の経過による背景変化）
  // 夜景の色
  const ColorF kNightColor1{ 0.1, 0.1, 0.3 };       // 深い青（上）
  const ColorF kNightColor2{ 0.3, 0.1, 0.4 };       // 紫（下）
  
  // 昼の色
  const ColorF kDayColor1{ 0.5, 0.7, 1.0 };         // 明るい青（上）
  const ColorF kDayColor2{ 0.7, 0.5, 0.3 };         // 明るいオレンジ（下）
  
  const ColorF kTitleColor{ 1.0, 1.0, 0.3 };            // 黄色
  const ColorF kTitleShadowColor{ 0.3, 0.1, 0.0, 0.8 }; // 影
  const ColorF kSubtitleColor{ 1.0, 1.0, 1.0 };         // 白
  const ColorF kMenuDefaultColor{ 0.8, 0.8, 0.8 };      // 非選択メニュー色
  const ColorF kMenuSelectedColor{ 1.0, 1.0, 0.3 };     // 選択中メニュー色（黄色）
  const ColorF kMenuShadowColor{ 0.2, 0.2, 0.2, 0.8 };  // メニュー影色

  // 位置設定
  const Vec2 kTitleShadowOffset{ 4.0, 4.0 };
  const Vec2 kMenuShadowOffset{ 2.0, 2.0 };
  constexpr int32 kTitleY = 200;
  constexpr int32 kSubtitleY = 340;
  constexpr int32 kMenuStartY = 420;        // メニュー開始Y座標
  constexpr int32 kMenuItemSpacing = 60;     // メニュー項目間隔

  // アニメーション設定
  constexpr double kTitlePulseInterval = 2.0;     // タイトルの脈動間隔（秒）
  constexpr double kDayNightCycleInterval = 20.0; // 昼夜サイクルの周期（秒）
  constexpr double kMenuItemScaleFactor = 1.1;    // 選択中メニューのスケール係数
  
  // 波のアニメーション設定
  constexpr int32 kWaveCount = 40;                // 波の数
  constexpr double kWaveSpeed = 0.5;              // 波の速度
  constexpr double kWaveAmplitude = 30.0;         // 波の振幅
  constexpr int32 kWaveSegments = 100;            // 波の分割数（滑らかさ）
  
  // 流れ星のアニメーション設定
  constexpr int32 kShootingStarCount = 15;        // 流れ星の数
  constexpr double kShootingStarSpeed = 200.0;    // 流れ星の速度（ピクセル/秒）
  constexpr double kShootingStarInterval = 0.5;   // 流れ星の発生間隔（秒）
  constexpr double kShootingStarLifetime = 3.0;   // 流れ星の寿命（秒）
  constexpr double kShootingStarTailLength = 100.0; // 流れ星の尾の長さ
  
  // クレジット情報
  const String kCreditsTitle = U"クレジット";
  const Array<String> kCreditsLines = {
    U"",
    U"ゲームデザイン・プログラム",
    U"  あなたの名前",
    U"",
    U"グラフィック",
    U"  Kenney (www.kenney.nl)",
    U"",
    U"フォント",
    U"  Potta One (Google Fonts)",
    U"",
    U"音楽・効果音",
    U"  あなたの名前",
    U"",
    U"制作ツール",
    U"  Siv3D",
    U"  Visual Studio",
    U"",
    U"スペシャルサンクス",
    U"  あなた！",
    U"",
    U"Escキーで戻る"
  };
}

// コンストラクタ（必ず実装する）
Title::Title(const InitData& init)
  : IScene{ init }
  , title_font_(FontMethod::MSDF, TitleConstants::kTitleFontSize, TitleConstants::kFontPath, FontStyle::Bold)
  , subtitle_font_(FontMethod::MSDF, TitleConstants::kSubtitleFontSize, TitleConstants::kFontPath)
  , menu_font_(FontMethod::MSDF, TitleConstants::kMenuFontSize, TitleConstants::kFontPath, FontStyle::Bold)
  , current_menu_item_(MenuItem::kGameStart)
  , show_credits_(false)
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
  // クレジット表示中の場合
  if (show_credits_) {
    // Escキーでクレジットを閉じる
    if (KeyEscape.down()) {
      show_credits_ = false;
      AudioManager::GetInstance()->PlaySe(SeKind::kCompleteWord);
    }
    return;
  }

  // 上キーでメニュー項目を上に移動
  if (KeyUp.down()) {
    switch (current_menu_item_) {
      case MenuItem::kGameStart:
        current_menu_item_ = MenuItem::kExit;
        break;
      case MenuItem::kCredits:
        current_menu_item_ = MenuItem::kGameStart;
        break;
      case MenuItem::kExit:
        current_menu_item_ = MenuItem::kCredits;
        break;
    }
    AudioManager::GetInstance()->PlaySe(SeKind::kTitleCursorMove);
  }

  // 下キーでメニュー項目を下に移動
  if (KeyDown.down()) {
    switch (current_menu_item_) {
      case MenuItem::kGameStart:
        current_menu_item_ = MenuItem::kCredits;
        break;
      case MenuItem::kCredits:
        current_menu_item_ = MenuItem::kExit;
        break;
      case MenuItem::kExit:
        current_menu_item_ = MenuItem::kGameStart;
        break;
    }
    AudioManager::GetInstance()->PlaySe(SeKind::kTitleCursorMove);
  }

  // Enterキーまたはスペースキーで選択
  if (KeyEnter.down() || KeySpace.down()) {
    stopwatch_.pause();

    switch (current_menu_item_) {
      case MenuItem::kGameStart:
        // ゲーム開始
        changeScene(EnumScene::kInGame);
        AudioManager::GetInstance()->PlaySe(SeKind::kTitleDecide);
        break;
      
      case MenuItem::kCredits:
        // クレジット表示
        show_credits_ = true;
        AudioManager::GetInstance()->PlaySe(SeKind::kTitleDecide);
        break;
      
      case MenuItem::kExit:
        // ゲーム終了
        System::Exit();
        break;
    }
  }
}

// 描画関数
void Title::draw() const
{
  // 経過時間を取得
  const double t = stopwatch_.sF();
  
  // 昼夜サイクルの進行度を計算（0.0→1.0→0.0のループ）
  const double cycle_progress = std::fmod(t, TitleConstants::kDayNightCycleInterval) / TitleConstants::kDayNightCycleInterval;
  
  // 夜→昼→夜の滑らかな遷移
  // 0.0-0.5: 夜→昼
  // 0.5-1.0: 昼→夜
  double day_night_factor;
  if (cycle_progress < 0.5) {
    // 夜から昼へ（0.0 → 1.0）
    day_night_factor = cycle_progress * 2.0;
  } else {
    // 昼から夜へ（1.0 → 0.0）
    day_night_factor = (1.0 - cycle_progress) * 2.0;
  }
  
  // 滑らかな補間（イージング）
  const double eased_factor = (1.0 - Cos(day_night_factor * Math::Pi)) * 0.5;
  
  // 背景色を補間
  const ColorF background_color1 = TitleConstants::kNightColor1.lerp(TitleConstants::kDayColor1, eased_factor);
  const ColorF background_color2 = TitleConstants::kNightColor2.lerp(TitleConstants::kDayColor2, eased_factor);
  
  // 背景グラデーション
  const Rect background_rect{ 0, 0, Scene::Width(), Scene::Height() };
  background_rect.draw(Arg::top = background_color1, Arg::bottom = background_color2);
  
  // 波のアニメーション（背景）
  for (int wave_index = 0; wave_index < TitleConstants::kWaveCount; ++wave_index) {
    // 各波のパラメータ
    const double wave_offset = wave_index * 20.0;
    const double wave_phase = wave_index * 0.3;
    const double wave_speed_factor = 1.0 + wave_index * 0.05;
    const double base_y = Scene::Height() * 0.3 + wave_offset;
    
    // 波の色（昼夜に応じて変化）
    const double hue = 200.0 + eased_factor * 40.0 + wave_index * 3.0;  // 青系→暖色系
    const double saturation = 0.5 + eased_factor * 0.3;  // 彩度も変化
    const double value = 0.5 + eased_factor * 0.3;       // 明度も変化
    const double alpha = 0.12 - wave_index * 0.0025;
    const ColorF wave_color = HSV{ hue, saturation, value, alpha };
    
    // 波の頂点を生成
    Array<Vec2> wave_points;
    wave_points.reserve(TitleConstants::kWaveSegments + 2);
    
    // 左端の開始点
    wave_points.push_back(Vec2{ 0, Scene::Height() });
    
    // 波の形状を計算
    for (int i = 0; i <= TitleConstants::kWaveSegments; ++i) {
      const double x = (Scene::Width() * i) / static_cast<double>(TitleConstants::kWaveSegments);
      
      // Sin波とCos波を組み合わせて複雑な波形を作成
      const double sin_wave = Sin((x * 0.01 + t * TitleConstants::kWaveSpeed * wave_speed_factor + wave_phase) * Math::TwoPi);
      const double cos_wave = Cos((x * 0.015 - t * TitleConstants::kWaveSpeed * wave_speed_factor * 0.7 + wave_phase * 1.5) * Math::TwoPi);
      
      const double y = base_y + 
                      sin_wave * TitleConstants::kWaveAmplitude +
                      cos_wave * TitleConstants::kWaveAmplitude * 0.5;
      
      wave_points.push_back(Vec2{ x, y });
    }
    
    // 右端の終了点
    wave_points.push_back(Vec2{ Scene::Width(), Scene::Height() });
    
    // 波を描画
    Polygon{ wave_points }.draw(wave_color);
  }

  // 流れ星エフェクト
  for (int i = 0; i < TitleConstants::kShootingStarCount; ++i) {
    // 各流れ星の開始時刻を計算
    const double star_start_time = i * TitleConstants::kShootingStarInterval;
    
    // 現在の流れ星のループ内時刻を計算
    const double loop_duration = TitleConstants::kShootingStarCount * TitleConstants::kShootingStarInterval;
    const double star_time = std::fmod(t - star_start_time + loop_duration, loop_duration);
    
    // 流れ星の寿命内かチェック
    if (star_time < TitleConstants::kShootingStarLifetime) {
      // 流れ星の開始位置と終了位置
      const double start_x = Scene::Width() * 0.2 + (i % 3) * Scene::Width() * 0.3;
      const double start_y = -100.0 + (i % 4) * 50.0;
      const double end_x = start_x + Scene::Width() * 0.6;
      const double end_y = Scene::Height() + 100.0;
      
      // 現在の位置を計算（線形補間）
      const double progress = star_time / TitleConstants::kShootingStarLifetime;
      const double current_x = start_x + (end_x - start_x) * progress;
      const double current_y = start_y + (end_y - start_y) * progress;
      
      // 流れ星の尾の開始位置を計算
      const double angle = Math::Atan2(end_y - start_y, end_x - start_x);
      const double tail_x = current_x - Cos(angle) * TitleConstants::kShootingStarTailLength;
      const double tail_y = current_y - Sin(angle) * TitleConstants::kShootingStarTailLength;
      
      // フェードイン・フェードアウト効果
      double alpha;
      if (progress < 0.2) {
        // フェードイン（最初の20%）
        alpha = progress / 0.2;
      } else if (progress > 0.8) {
        // フェードアウト（最後の20%）
        alpha = (1.0 - progress) / 0.2;
      } else {
        // 完全に表示
        alpha = 1.0;
      }
      
      // 流れ星の色（黄色から白のグラデーション）
      const ColorF star_color_head{ 1.0, 1.0, 1.0, alpha * 0.9 };
      const ColorF star_color_tail{ 1.0, 1.0, 0.6, alpha * 0.3 };
      
      // 流れ星の本体（線）
      Line{ tail_x, tail_y, current_x, current_y }
        .draw(3.0, star_color_tail, star_color_head);
      
      // 流れ星の頭部（円）
      Circle{ current_x, current_y, 5.0 }.draw(star_color_head);
      Circle{ current_x, current_y, 3.0 }.draw(ColorF{ 1.0, 1.0, 1.0, alpha });
    }
  }

  // クレジット表示中の場合
  if (show_credits_) {
    // 半透明の黒背景
    Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, 0.8 });
    
    // クレジット内容を表示
    const int32 start_y = 80;
    const int32 line_height = 32;
    
    // タイトル
    title_font_(TitleConstants::kCreditsTitle).drawAt(Scene::Center().x, start_y, TitleConstants::kTitleColor);
    
    // 各行を表示
    for (size_t i = 0; i < TitleConstants::kCreditsLines.size(); ++i) {
      const int32 y = start_y + 80 + static_cast<int32>(i) * line_height;
      menu_font_(TitleConstants::kCreditsLines[i]).drawAt(Scene::Center().x, y, ColorF{ 1.0, 1.0, 1.0 });
    }
  }
  // 通常のタイトル画面表示
  else {
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

    // メニュー項目を描画
    const Array<std::pair<MenuItem, String>> menu_items = {
      { MenuItem::kGameStart, TitleConstants::kMenuGameStart },
      { MenuItem::kCredits, TitleConstants::kMenuCredits },
      { MenuItem::kExit, TitleConstants::kMenuExit }
    };

    for (size_t i = 0; i < menu_items.size(); ++i) {
      const auto& [item, text] = menu_items[i];
      const bool is_selected = (item == current_menu_item_);
      const int32 y = TitleConstants::kMenuStartY + static_cast<int32>(i) * TitleConstants::kMenuItemSpacing;
      const Vec2 menu_pos{ Scene::Center().x, y };

      // 選択中の項目は拡大
      const double scale = is_selected ? TitleConstants::kMenuItemScaleFactor : 1.0;
      const ColorF color = is_selected ? TitleConstants::kMenuSelectedColor : TitleConstants::kMenuDefaultColor;

      // 影
      menu_font_(text).drawAt(menu_pos + TitleConstants::kMenuShadowOffset, TitleConstants::kMenuShadowColor);

      // 本体
      {
        const Transformer2D transform{ Mat3x2::Scale(scale, menu_pos) };
        menu_font_(text).drawAt(menu_pos, color);
      }

      // 選択中の項目には矢印を表示
      if (is_selected) {
        const double arrow_offset = 150.0 + 10.0 * Sin(t * 6.0);
        const Vec2 left_arrow_pos{ menu_pos.x - arrow_offset, menu_pos.y };
        const Vec2 right_arrow_pos{ menu_pos.x + arrow_offset, menu_pos.y };
        
        // 丸い印（やわらかい感じ）
        const double circle_radius = 8.0 + 2.0 * Sin(t * 8.0);  // 脈動する丸
        Circle{ left_arrow_pos, circle_radius }.draw(color);
        Circle{ right_arrow_pos, circle_radius }.draw(color);
        
        // より小さな白い丸を中に描画してハイライト効果
        const double inner_radius = circle_radius * 0.5;
        Circle{ left_arrow_pos, inner_radius }.draw(ColorF{ 1.0, 1.0, 1.0, 0.6 });
        Circle{ right_arrow_pos, inner_radius }.draw(ColorF{ 1.0, 1.0, 1.0, 0.6 });
      }
    }
  }

  // 明るさ設定を適用
  GameSettings::GetInstance()->ApplyBrightness();
}

void Title::drawFadeIn(double t) const
{
  draw();

  // シンプルな黒フェードイン
  Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, 1.0 - t });
}

void Title::drawFadeOut(double t) const
{
  draw();

  // シンプルな黒フェードアウト
  Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, t });
}
