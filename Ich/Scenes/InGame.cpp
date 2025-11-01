#include "stdafx.h"

#include "InGame.h"

#include "System/Audio/AudioManager.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/GameSettings.h"
#include "System/System/BlockManager.h"
#include "Keywords.hpp"
#include "Messages.hpp"

namespace InGameConstants
{
  const Vec2 kStartBlock{ 200, 200 };

  // ブロックサイズ
  constexpr int32 kBlockSize = 100;
  constexpr int32 kStartX = 20;
  constexpr int32 kStartY = 250;
  constexpr int32 kWallStartY = 0;               // 壁ブロックの開始Y座標（画面最上部）

  // プレイヤーの物理パラメータ
  constexpr float kGravity = 800.0f;              // ピクセル/秒^2
  constexpr float kMaxFallSpeed = 600.0f;         // 最大落下速度
  constexpr float kPlayerMoveSpeed = 200.0f;      // プレイヤーの移動速度

  // カメラパラメータ
  constexpr float kCameraFollowSpeed = 0.1f;      // カメラ追従速度（0.0～1.0）
  // プレイヤーの下限の見え方を上げるための縦バイアス（ブロック数）
  constexpr int32 kCameraVerticalBiasBlocks = 2;  // ブロック2個分だけ上げる

  // 文字収集パラメータ
  constexpr size_t kMaxCharacters = 5;            // 最大文字数
  // UIパラメータ
  constexpr int32 kAirGaugeX = 900;               // エアゲージX座標
  constexpr int32 kAirGaugeY = 50;                // エアゲージY座標
  constexpr int32 kSideBoxX = 880;                // サイドボックスX座標
  constexpr int32 kSideBoxY = 120;                // サイドボックスY座標

  // ブロックグリッドパラメータ
  constexpr int32 kGridRows = 6;                 // グリッド行数
  constexpr int32 kGridColumns = 6;               // グリッド列数
  constexpr int32 kWallThickness = 1;             // 壁の厚さ（ブロック数）

  // プレイヤー初期位置
  constexpr int32 kPlayerInitialX = 200;
  constexpr int32 kPlayerInitialY = 20;

  // もじぴったん風UIパラメータ
  constexpr int32 kCharBoxSize = 50;              // 各文字ボックスのサイズ
  constexpr int32 kCharBoxSpacing = 5;            // ボックス間の隙間
  constexpr int32 kCharBoxStartX = 50;            // 開始X位置
  constexpr int32 kCharBoxStartY = 20;            // 開始Y位置

  // 完成した単語ボードパラメータ
  constexpr int32 kCompletedBoardX = 850;
  constexpr int32 kCompletedBoardY = 80;
  constexpr int32 kCompletedBoardWidth = 400;
  constexpr int32 kCompletedBoardHeight = 550;
  constexpr int32 kCompletedBoardLineHeight = 18;

  constexpr double kHintUpdateInterval = 3.0;
  constexpr double kMessageDisplayDuration = 3.0;

  // Block destruction parameters
  constexpr float kBlockDestroyVerticalThreshold = 10.0f;  // Threshold for upward block destruction
  // Simple gravity parameters
  constexpr float kSimpleGravity = 4.0f;          // Simple gravity used by UpdatePlayerMovement
  constexpr float kGravityMargin = 5.0f;          // Margin applied during gravity collision checks

  // 画面サイズ
  constexpr int32 kScreenWidth = 1280;
  constexpr int32 kScreenHeight = 720;

  // シード計算用の定数
  constexpr uint64 kSeedMultiplierRow = 982451653ULL;
  constexpr uint64 kSeedMultiplierCol = 1572869ULL;

  // 影オフセット
  const Vec2 kBlockTextShadowOffset{ 3.0, 3.0 };
  const Vec2 kCharBoxTextShadowOffset{ 2.0, 2.0 };

  // 色定義
  const ColorF kBlockTextShadowColor{ 0.0, 0.0, 0.0, 0.9 };
  const ColorF kBlockTextColor{ 1.0 };
  const ColorF kCharBoxTextShadowColor{ 0.0, 0.0, 0.0, 0.3 };
  const ColorF kCharBoxDefaultTextColor{ 0.0, 0.0, 0.0 };
  const ColorF kCharBoxCompletedTextColor{ 0.8, 0.0, 0.0 };
  const ColorF kCharBoxDefaultBoxColor{ 1.0, 1.0, 1.0 };
  const ColorF kCharBoxCompletedBoxColor{ 1.0, 0.8, 0.8 };
  const ColorF kCharBoxDefaultBorderColor{ 0.3, 0.3, 0.3 };
  const ColorF kCharBoxCompletedBorderColor{ 1.0, 0.0, 0.0 };
  const ColorF kBlockFrameColor{ 0.2, 0.2, 0.2, 0.5 };
  const ColorF kCompletedBoardBackgroundColor{ 0.1, 0.1, 0.1, 0.8 };
  const ColorF kCompletedBoardBorderColor{ 0.8, 0.8, 0.8 };
  const ColorF kCompletedBoardTitleColor{ 1.0, 1.0, 0.0 };
  const ColorF kCompletedWordTextColor{ 0.0, 1.0, 0.0 };
  const ColorF kCompletedBoardOverlayColor{ 1.0, 1.0, 1.0, 0.2 };
  const ColorF kBackgroundColor{ 0.6, 0.8, 0.7 };
  const ColorF kHintBackgroundColor{ 1.0, 1.0, 1.0, 0.92 };
  const ColorF kHintBorderColor{ 0.2, 0.3, 0.5, 0.9 };
  const ColorF kHintTextColor{ 0.1, 0.1, 0.1 };
  const ColorF kDebugLineColorBlue = Palette::Blue;
  const ColorF kDebugLineColorOrange = Palette::Orange;
  const ColorF kDebugFrameColorRed = Palette::Red;
  const ColorF kDebugCircleColorRed = Palette::Red;
  const ColorF kDebugTextColorWhite = Palette::White;

  // 角丸半径
  constexpr double kBlockRoundRadius = 15.0;
  constexpr double kCharBoxRoundRadius = 5.0;
  constexpr double kCompletedBoardRoundRadius = 10.0;
  constexpr double kHintBoxRoundRadius = 18.0;
  constexpr double kWeaponRoundRadius = 10.0;

  // 線の太さ
  constexpr double kBlockFrameThickness = 2.0;
  constexpr double kCharBoxFrameThickness = 3.0;
  constexpr double kCompletedBoardFrameThickness = 3.0;
  constexpr double kHintBoxFrameThickness = 2.0;
  constexpr double kDebugLineThickness = 2.0;
  constexpr double kDebugFrameThickness = 2.0;
  constexpr double kHintBubbleFrameThickness1 = 1.5;
  constexpr double kHintBubbleFrameThickness2 = 1.2;

  // ヒントバブルパラメータ
  const Vec2 kHintBoxOffset{ 80.0, -70.0 }; // 20px 下へ
  const Vec2 kHintBubbleAnchorOffset{ 50.0, -30.0 }; // 20px 下へ
  const Vec2 kHintBubble2Offset{ -15.0, 15.0 };
  constexpr double kHintBoxPadding = 18.0;
  constexpr double kHintBubble1Radius = 8.0;
  constexpr double kHintBubble2Radius = 5.0;

  // デバッグ表示パラメータ
  constexpr int32 kDebugTextX = 20;
  constexpr int32 kDebugTextY1 = 20;
  constexpr int32 kDebugTextY2 = 40;
  constexpr int32 kDebugTextY3 = 60;
  constexpr double kDebugCircleRadius = 3.0;

  // 完成単語ボードパラメータ
  constexpr int32 kCompletedBoardColumns = 4;
  constexpr double kCompletedBoardColumnPadding = 12.0;
  constexpr int32 kCompletedBoardTitleOffsetY = 30;
  constexpr int32 kCompletedBoardContentStartY = 60;
  constexpr int32 kCompletedBoardCountOffsetX = 10;
  constexpr int32 kCompletedBoardCountOffsetY = 25;
  constexpr double kCompletedBoardOverlayScale = 3.6;

  // エア量調整パラメータ

  /// <summary>
  /// エア回復単位
  /// </summary>
  constexpr float kAirDecreaseRate = 0.005f;

  // フォントサイズ
  constexpr int32 kBlockFontSize = 40;
  constexpr int32 kCompletedWordFontSize = 16;
  constexpr int32 kHintFontSize = 20;
  constexpr int32 kDebugFontSize = 16;

  // ゲームオーバーパラメータ
  constexpr double kGameOverDuration = 3.0;       // ゲームオーバー演出の持続時間
  constexpr int32 kGameOverFontSize = 60;         // ゲームオーバーフォントサイズ
  const ColorF kGameOverTextColor{ 1.0, 0.2, 0.2 };  // ゲームオーバーテキスト色

  // ゲームクリアパラメータ
  constexpr double kGameClearDuration = 3.0;      // ゲームクリア演出の持続時間
  constexpr int32 kGameClearFontSize = 60;        // ゲームクリアフォントサイズ
  const ColorF kGameClearTextColor{ 1.0, 0.8, 0.2 };  // ゲームクリアテキスト色（金色）

  /// <summary>
  /// ブロックのイメージパス
  /// </summary>
  const Array<String> kBlockTexturePaths = {
    U"Assets/Image/block_blue.jpg",
    U"Assets/Image/block_green.jpg",
    U"Assets/Image/block_orange.jpg",
    U"Assets/Image/block_purple.jpg",
    U"Assets/Image/block_yellow.jpg"
  };

  /// <summary>
  /// ブロックのイメージパス
  /// </summary>
  const String kBlockBgTexturePath = U"Assets/Image/block_bg.png";

  /// <summary>
  /// ブロックCOLOR
  /// </summary>
  const Array<ColorF> kBlockColors = {
    ColorF{ 0.0, 0.909, 1.0 },  // 青
    ColorF{ 0.3, 1.0, 0.3 },  // 緑
    ColorF{ 1.0, 0.6, 0.3 },  // オレンジ
    ColorF{ 1.0, 0.3, 1.0 },  // マゼンタ
    ColorF{ 1.0, 1.0, 0.3 },  // 黄色
  };

  // 壁ブロックの色
  const ColorF kWallBlockColor{ 0.3, 0.3, 0.3 };  // ダークグレー
  const ColorF kWallBlockFrameColor{ 0.15, 0.15, 0.15, 0.8 }; // より暗いグレー
  const ColorF kWallPatternColor{ 0.2, 0.2, 0.2, 0.3 }; // パターンの色
  constexpr double kWallPatternLineSpacing = 15.0; // 壁パターンの線間隔
  constexpr double kWallPatternLineThickness = 1.5; // 壁パターンの線の太さ

  /// <summary>
  /// エア最大値
  /// </summary>
  const float kAirMax = 1.0f;

  /// <summary>
  /// 1ブロック当たりのエア消費量
  /// </summary>
  const float kAirConsumeRate = 0.02f;

  /// <summary>
  /// 単語を作成したときのエア回復量
  /// </summary>
  const float kAirRecoverRate = 0.1f;

  /// <summary>
  /// クリアまでの単語の完成数
  /// </summary>
  const int kClearEvaluationCount = 50;
}

Game::Game(const InitData& init)
  : IScene{ init }
  , block_bg_texture_(InGameConstants::kBlockBgTexturePath)
  , menu_(std::make_unique<Menu>())
  , ui_(std::make_shared<Ui>())
  , player_(std::make_shared<Player>())
  , block_destroy_effect_(std::make_unique<BlockDestroyEffect>())
  , bubble_effect_(std::make_unique<BubbleEffect>())
  , word_display_box_(std::make_unique<WordDisplayBox>())
  , collected_characters_(std::make_unique<CollectedCharacters>())
  , air_amount_(1.0f)
  , is_game_over_(false)
  , is_game_clear_(false)
  , game_over_timer_(0.0)
  , game_clear_timer_(0.0)
  , block_font_{ InGameConstants::kBlockFontSize, Typeface::Bold }
  , completed_word_font_{ InGameConstants::kCompletedWordFontSize }
  , hint_font_{ InGameConstants::kHintFontSize }
  , debug_font_{ InGameConstants::kDebugFontSize }
  , game_over_font_{ InGameConstants::kGameOverFontSize, Typeface::Bold }
{
  // BGM 再生（シーン開始時）
  AudioManager::GetInstance()->PlayBgm(BgmKind::kMain);
  auto& shared_data = getData<SaveData>();
  shared_data.last_game_cleared_ = false;
  shared_data.last_completed_words_.clear();


  // ? テクスチャを生成（絵文字）
  sparkle_tex_ = Texture{ Emoji{ U"✨" } };
  // 武器テクスチャを読み込み
  weapon_texture_ = Texture{ U"Assets/Image/pen.png" };

  for (const auto& path : InGameConstants::kBlockTexturePaths) {
    Texture texture{ path };
    if (texture.isEmpty()) {
      PRINT << U"Failed to load block texture: " << path;
      continue;
    }
    block_textures_ << texture;
  }


  // UIの初期設定（1280x720対応）
  ui_->SetAirGaugePosition(InGameConstants::kAirGaugeX, InGameConstants::kAirGaugeY);
  ui_->SetAirGauge(air_amount_);

  // サイドボックスを画面右下に配置（1280x720対応）
  ui_->SetSideBoxPosition(InGameConstants::kSideBoxX, InGameConstants::kSideBoxY);
  ui_->SetSideBoxVisible(true);

  // ブロックグリッドを生成（10行x6列、バッチサイズ20）
  const Array<Array<std::pair<String, bool>>> string_grid = block_manager_.GenerateBlockGrid(
    InGameConstants::kGridRows,
    InGameConstants::kGridColumns,
    30,
    10,
    keywords,
    3,
    false
  );

  // String配列をBlock配列に変換（壁を含む拡張グリッドを作成）
  const int32 total_columns = InGameConstants::kGridColumns + InGameConstants::kWallThickness * 2;

  // 初期の壁の高さ（画面最上部から最初のブロック塊の下端まで）
  const int32 initial_wall_height = static_cast<int32>(std::ceil((InGameConstants::kStartY + string_grid.size() * InGameConstants::kBlockSize - InGameConstants::kWallStartY) / static_cast<float>(InGameConstants::kBlockSize)));

  // 現在のブロック塊の下端Y座標を記録
  current_chunk_bottom_y_ = InGameConstants::kStartY + string_grid.size() * InGameConstants::kBlockSize;
  
  // 次のブロック塊を生成するトリガー位置を設定
  next_chunk_trigger_y_ = current_chunk_bottom_y_ - kChunkSpacing;

  block_grid_.resize(initial_wall_height);

  for (size_t row = 0; row < static_cast<size_t>(initial_wall_height); ++row) {
    block_grid_[row].resize(total_columns);

    for (size_t col = 0; col < static_cast<size_t>(total_columns); ++col) {
      const bool is_wall_column = (col < InGameConstants::kWallThickness || col >= static_cast<size_t>(InGameConstants::kGridColumns + InGameConstants::kWallThickness));
      
      // 左右の壁（画面最上部から開始）
      if (is_wall_column) {
        block_grid_[row][col] = Block(Block::Type::kWall);
        // 壁ブロックは画面最上部から配置
        const float wall_x = InGameConstants::kStartX + static_cast<int32>(col) * InGameConstants::kBlockSize;
        const float wall_y = InGameConstants::kWallStartY + static_cast<int32>(row) * InGameConstants::kBlockSize;
        block_grid_[row][col].position = Vec2{ wall_x, wall_y };
      }
      // 通常のブロック領域
      else {
        // 通常ブロック領域の行インデックスを計算
        const int32 normal_block_row_offset = static_cast<int32>((InGameConstants::kStartY - InGameConstants::kWallStartY) / InGameConstants::kBlockSize);
        
        if (static_cast<int32>(row) >= normal_block_row_offset && static_cast<int32>(row) < normal_block_row_offset + static_cast<int32>(string_grid.size())) {
          const size_t actual_row = static_cast<size_t>(static_cast<int32>(row) - normal_block_row_offset);
          const size_t actual_col = static_cast<size_t>(static_cast<int32>(col) - InGameConstants::kWallThickness);
          const auto& cell = string_grid[actual_row][actual_col];
          block_grid_[row][col] = Block(cell.first, cell.second);
          // 通常ブロックも壁座標系で位置を設定
          const float block_x = InGameConstants::kStartX + static_cast<int32>(col) * InGameConstants::kBlockSize;
          const float block_y = InGameConstants::kStartY + static_cast<int32>(actual_row) * InGameConstants::kBlockSize;
          block_grid_[row][col].position = Vec2{ block_x, block_y };
          const size_t seed = (row * InGameConstants::kSeedMultiplierRow + col * InGameConstants::kSeedMultiplierCol);
          block_grid_[row][col].color = InGameConstants::kBlockColors[seed % InGameConstants::kBlockColors.size()];
          //block_grid_[row][col].color = InGameConstants::kBlockColors[(actual_row + actual_col) % InGameConstants::kBlockColors.size()];
        }
        else {
          // 通常ブロック領域外は空ブロック
          block_grid_[row][col] = Block();
          const float empty_x = InGameConstants::kStartX + static_cast<int32>(col) * InGameConstants::kBlockSize;
          const float empty_y = InGameConstants::kWallStartY + static_cast<int32>(row) * InGameConstants::kBlockSize;
          block_grid_[row][col].position = Vec2{ empty_x, empty_y };
        }
      }
    }
  }

  // プレイヤーの初期設定（グリッドの一番上の中央に配置）
  const int32 normal_block_row_offset = static_cast<int32>((InGameConstants::kStartY - InGameConstants::kWallStartY) / InGameConstants::kBlockSize);
  const int32 initial_col = InGameConstants::kGridColumns / 2 + InGameConstants::kWallThickness;  // 中央（壁を考慮）
  const int32 initial_row = normal_block_row_offset;  // 通常ブロック領域の一番上
  const Vec2 initial_pos = Vec2{
    InGameConstants::kStartX + initial_col * InGameConstants::kBlockSize + InGameConstants::kBlockSize / 2.0f,
    InGameConstants::kWallStartY + initial_row * InGameConstants::kBlockSize + InGameConstants::kBlockSize / 2.0f
  };

  //player_->SetPosition(initial_pos.x, initial_pos.y);
  player_->SetPosition(InGameConstants::kPlayerInitialX, InGameConstants::kPlayerInitialY);
  player_->SetMoveSpeed(InGameConstants::kPlayerMoveSpeed);  // 移動速度を200ピクセル/秒に設定

  // CollectedCharactersの初期化（互換性のためhave_words_も維持）
  collected_characters_->SetMaxSize(max_string_);
  for (size_t i = 0; i < max_string_; i++) {
    have_words_.push_back(U"");
  }

  // WordDisplayBoxの設定
  if (word_display_box_)
  {
    word_display_box_->SetPosition(InGameConstants::kCharBoxStartX, InGameConstants::kCharBoxStartY);
    word_display_box_->SetBoxSize(InGameConstants::kCharBoxSize);
    word_display_box_->SetBoxSpacing(InGameConstants::kCharBoxSpacing);
  }

  UpdateHint();
}
Game::~Game()
{
  //PRINT << U"Game::~Game()";
}

bool Game::PixelToGrid(const Vec2& pixelPos, int32& gridRow, int32& gridCol) const
{
  // ピクセル座標からグリッド座標を計算（壁用の座標系を使用）
  const float relative_x = pixelPos.x - InGameConstants::kStartX;
  const float relative_y = pixelPos.y - InGameConstants::kWallStartY;

  gridCol = static_cast<int32>(relative_x / InGameConstants::kBlockSize);
  gridRow = static_cast<int32>(relative_y / InGameConstants::kBlockSize);

  // グリッドの範囲内かチェック
  if (gridRow < 0 || gridRow >= static_cast<int32>(block_grid_.size())) {
    return false;
  }
  if (gridCol < 0 || gridCol >= static_cast<int32>(block_grid_[0].size())) {
    return false;
  }

  return true;
}

Vec2 Game::GridToPixel(int32 gridRow, int32 gridCol) const
{
  // グリッド座標からピクセル座標（中心）を計算
  const float pixel_x = InGameConstants::kStartX + gridCol * InGameConstants::kBlockSize + InGameConstants::kBlockSize / 2.0f;
  const float pixel_y = InGameConstants::kStartY + gridRow * InGameConstants::kBlockSize + InGameConstants::kBlockSize / 2.0f;
  return Vec2{ pixel_x, pixel_y };
}

Vec2 Game::GetGridTopLeft(int32 gridRow, int32 gridCol) const
{
  // グリッドの左上座標を取得
  const float pixel_x = InGameConstants::kStartX + gridCol * InGameConstants::kBlockSize;
  const float pixel_y = InGameConstants::kStartY + gridRow * InGameConstants::kBlockSize;
  return Vec2{ pixel_x, pixel_y };
}

bool Game::GetPlayerGridPosition(int32& gridRow, int32& gridCol) const
{
  const Vec2 player_pos = player_->GetPosition();
  return PixelToGrid(player_pos, gridRow, gridCol);
}

void Game::DestroyBlockUnderPlayer()
{
  const Vec2 player_pos = player_->GetPosition();
  const float player_bottom_y = player_pos.y + player_->GetHeight() / 2.0f;
  const float player_left = player_pos.x - player_->GetWidth() / 2.0f;
  const float player_right = player_pos.x + player_->GetWidth() / 2.0f;
  const float player_top = player_pos.y - player_->GetHeight() / 2.0f;

  // プレイヤーの周囲のブロックを探す
  for (size_t i = 0; i < block_grid_.size(); i++) {
    for (size_t j = 0; j < block_grid_[i].size(); j++) {
      Block& block = block_grid_[i][j];

      // 空、破壊済み、または壁ブロックはスキップ
      if (block.isEmpty() || block.isWall()) {
        continue;
      }

      const Vec2 block_pos = block.position;
      const float block_left = block_pos.x;
      const float block_right = block_pos.x + InGameConstants::kBlockSize;
      const float block_top = block_pos.y;
      const float block_bottom = block_pos.y + InGameConstants::kBlockSize;

      bool can_destroy = false;
      String direction;

      // 下のブロック（足元）
      if (player_pos.x >= block_left && player_pos.x <= block_right) {
        if (player_bottom_y >= block_top && player_bottom_y <= block_top + InGameConstants::kBlockDestroyVerticalThreshold) {
          if (!KeyLeft.pressed() && !KeyRight.pressed()) {
            can_destroy = true;
            direction = U"下";
          }
        }
      }

      // 左のブロック
      if (KeyLeft.pressed() && player_left >= block_left && player_left <= block_right) {
        if (player_pos.y >= block_top && player_pos.y <= block_bottom) {
          can_destroy = true;
          direction = U"左";
        }
      }

      // 右のブロック
      if (KeyRight.pressed() && player_right >= block_left && player_right <= block_right) {
        if (player_pos.y >= block_top && player_pos.y <= block_bottom) {
          can_destroy = true;
          direction = U"右";
        }
      }

      // 上のブロック（頭上）
      if (player_pos.x >= block_left && player_pos.x <= block_right) {
        if (player_top <= block_bottom && player_top >= block_bottom - InGameConstants::kBlockDestroyVerticalThreshold) {
          can_destroy = true;
          direction = U"上";
        }
      }

      if (can_destroy) {
        // ブロックの中心位置を計算（block.positionから直接計算）
        const Vec2 block_center = block_pos + Vec2{ InGameConstants::kBlockSize / 2.0f, InGameConstants::kBlockSize / 2.0f };

        // ブロックの色を決定（位置依存のシード）
        const size_t seed = (i * InGameConstants::kSeedMultiplierRow + j * InGameConstants::kSeedMultiplierCol);
        const size_t color_count = InGameConstants::kBlockColors.size();
        const ColorF block_color = block_grid_[i][j].color;

        // 破壊演出を追加
        if (block_destroy_effect_) {
          block_destroy_effect_->AddEffect(block_center, block_color, block.value);
        }

        // ブロックを破壊
        block.is_destroyed = true;
        //PRINT << U"Block destroyed (" << direction << U") at row: " << i << U", col: " << j;

        // エアを消費
#if _DEBUG
        // デバッグモードでエア無限が有効な場合はエアを消費しない
        if (!menu_ || !menu_->IsInfiniteAirEnabled())
#endif
        {
          air_amount_ -= InGameConstants::kAirConsumeRate * static_cast<float>(std::sqrt(static_cast<float>(stage_)));
          if (air_amount_ < 0.0f) {
            air_amount_ = 0.0f;
          }
        }

        // 文字を追加（新しいシステム）
        if (collected_characters_)
        {
          collected_characters_->AddCharacter(block.value);
        }

        // 文字を追加（互換性のため既存のシステムも維持）
        have_words_.push_back(block.value);

        // max_string_を超えたら先頭から削除
        while (have_words_.size() > max_string_) {
          have_words_.erase(have_words_.begin());
        }

        hint_timer_ = 0.0;
        UpdateHint();

        AudioManager::GetInstance()->PlaySe(SeKind::kDestroyBlock);

        return;  // 1つだけ破壊して終了
      }
    }
  }

  // PRINT << U"No block found to destroy near player";
}

void Game::UpdatePlayerFall(float delta_time)
{
  int32 grid_row, grid_col;
  if (!GetPlayerGridPosition(grid_row, grid_col)) {
    return;
  }

  //// プレイヤーの現在位置のブロックをチェック
  const bool is_on_block = HasBlockAt(grid_row, grid_col);

  // プレイヤーの下のブロックをチェック
  const int32 below_row = grid_row + 1;
  bool has_block_below = false;

  if (below_row >= 0 && below_row < static_cast<int32>(block_grid_.size()) &&
    grid_col >= 0 && grid_col < static_cast<int32>(block_grid_[below_row].size())) {
    has_block_below = HasBlockAt(below_row, grid_col);  // isEmpty()ではなくHasBlockAt()を使用
  }

  // 下にブロックがない場合は落下
  if (!has_block_below && below_row < static_cast<int32>(block_grid_.size())) {
    player_fall_velocity_ += InGameConstants::kGravity * delta_time;
    player_fall_velocity_ = Min(player_fall_velocity_, InGameConstants::kMaxFallSpeed);

    Vec2 player_pos = player_->GetPosition();
    player_pos.y += player_fall_velocity_ * delta_time;

    // 下にブロックがあるかどうかを、ピクセル座標ベースでも確認
    const float player_bottom_y = player_pos.y + player_->GetHeight() / 2.0f;
    bool should_land = false;
    float landing_y = player_pos.y;

    // 全ブロックをチェックして衝突判定
    for (size_t i = 0; i < block_grid_.size(); i++) {
      for (size_t j = 0; j < block_grid_[i].size(); j++) {
        const Block& block = block_grid_[i][j];

        // 空または破壊されたブロックはスキップ（壁は衝突対象）
        if (block.isEmpty() && !block.isWall()) {
          continue;
        }

        const Vec2 block_pos = block.position;
        const float block_left = block_pos.x;
        const float block_right = block_pos.x + InGameConstants::kBlockSize;
        const float block_top = block_pos.y;
        const float block_bottom = block_pos.y + InGameConstants::kBlockSize;

        // プレイヤーの中心がブロックのX範囲内にあるかチェック
        if (player_pos.x >= block_left && player_pos.x <= block_right) {
          // プレイヤーの下端がブロックの上面に到達または超えた場合
          if (player_bottom_y >= block_top && player_pos.y < block_top) {
            should_land = true;
            landing_y = block_top - player_->GetHeight() / 2.0f;
            break;
          }
        }
      }
      if (should_land) {
        break;
      }
    }

    if (should_land) {
      player_pos.y = landing_y;
      player_fall_velocity_ = 0.0f;
      player_->SetPosition(player_pos.x, player_pos.y);
      player_->RefreshPoseFromMovement();
    } else {
      player_->SetPosition(player_pos.x, player_pos.y);
      player_->SetPose(Player::Pose::kFall);
    }
  } else {
    // ブロックがある場合は落下速度をリセット
    player_fall_velocity_ = 0.0f;
    player_->RefreshPoseFromMovement();
  }
}

bool Game::HasBlockAt(int32 gridRow, int32 gridCol) const
{
  // グリッドの範囲外チェック
  if (gridRow < 0 || gridRow >= static_cast<int32>(block_grid_.size())) {
    return false;
  }
  if (gridCol < 0 || gridCol >= static_cast<int32>(block_grid_[0].size())) {
    return false;
  }

  const Block& block = block_grid_[gridRow][gridCol];

  // 壁ブロックまたは通常の未破壊ブロックが存在する場合true
  return block.isWall() || !block.isEmpty();
}

void Game::UpdatePlayerMovement(float delta_time)
{
  // 移動入力を取得
  Vec2 move_input = Vec2::Zero();
  bool is_moving = false;
  bool facing_left = false;

  const bool walk_forward_left = KeyUp.pressed();
  const bool walk_forward_right = KeyDown.pressed();

  // 上下入力は「その場で向きを変えるだけ」なので歩行アニメーションには移行させず、待機ポーズを使用する。
  if (walk_forward_left || walk_forward_right) {
    player_->SetMoving(false);
    player_->SetPose(Player::Pose::kIdle);
  }

  if (KeyLeft.pressed()) {
    move_input.x = -1.0f;
    is_moving = true;
    facing_left = true;
  } else if (KeyRight.pressed()) {
    move_input.x = 1.0f;
    is_moving = true;
    facing_left = false;
  }

  // プレイヤーの移動状態と向きを更新
  player_->SetMoving(is_moving);
  if (is_moving) {
    player_->SetFacingLeft(facing_left);
  }

  // プレイヤーの現在位置を取得
  Vec2 player_pos = player_->GetPosition();
  Vec2 next_pos = player_pos;
  next_pos.y += InGameConstants::kSimpleGravity;

  const float player_bottom_y = next_pos.y + player_->GetHeight() / 2.0f;
  const float player_top = next_pos.y - player_->GetHeight() / 2.0f;
  const float player_half_width = player_->GetWidth() / 2.0f;
  bool is_on_block = false;

  // 重力による落下とブロック衝突判定
  for (int i = 0; i < block_grid_.size(); i++) {
    for (int j = 0; j < block_grid_[i].size(); j++) {
      const Block& block = block_grid_[i][j];

      // 空または破壊されたブロックはスキップ（壁は衝突対象）
      if (block.isEmpty() && !block.isWall()) {
        continue;
      }

      const Vec2 block_pos = block.position;
      const float block_left = block_pos.x;
      const float block_right = block_pos.x + InGameConstants::kBlockSize;
      const float block_top = block_pos.y;
      const float block_bottom = block_pos.y + InGameConstants::kBlockSize;

      // デバッグ用の線描画
      if (kDebugMode) {
        Line{ block_left, block_top, block_left, block_bottom }.draw(InGameConstants::kDebugLineThickness, InGameConstants::kDebugLineColorBlue);
        Line{ block_right, block_top, block_right, block_bottom }.draw(InGameConstants::kDebugLineThickness, InGameConstants::kDebugLineColorOrange);
      }

      // プレイヤーの中心がブロックのX範囲内にあるかチェック
      if (next_pos.x >= block_left && next_pos.x <= block_right) {
        // プレイヤーの下端がブロックの上面付近にあるかチェック
        if (player_bottom_y >= block_top && player_bottom_y <= block_top + InGameConstants::kSimpleGravity + InGameConstants::kGravityMargin) {
          // プレイヤーをブロックの上に配置
          next_pos.y = block_top - player_->GetHeight() / 2.0f;
          is_on_block = true;
          break;
        }
      }
    }

    if (is_on_block) {
      break;
    }
  }
  
  // 落下中にブロックにめり込みがないかチェック
  if (!is_on_block) {
    const float player_left = next_pos.x - player_half_width;
    const float player_right = next_pos.x + player_half_width;
    
    for (int i = 0; i < block_grid_.size(); i++) {
      for (int j = 0; j < block_grid_[i].size(); j++) {
        const Block& block = block_grid_[i][j];
        
        // 空または破壊されたブロックはスキップ（壁は衝突対象）
        if (block.isEmpty() && !block.isWall()) {
          continue;
        }
        
        const Vec2 block_pos = block.position;
        const float block_left = block_pos.x;
        const float block_right = block_pos.x + InGameConstants::kBlockSize;
        const float block_top = block_pos.y;
        const float block_bottom = block_pos.y + InGameConstants::kBlockSize;
        
        // Y方向の重なりをチェック
        const bool y_overlap = !(player_bottom_y <= block_top || player_top >= block_bottom);
        
        if (y_overlap) {
          // X方向の重なりをチェック
          const bool x_overlap = !(player_right <= block_left || player_left >= block_right);
          
          if (x_overlap) {
            // めり込み量を計算
            const float overlap_left = player_right - block_left;   // 左側からのめり込み
            const float overlap_right = block_right - player_left;  // 右側からのめり込み
            
            // めり込み量が少ない方向に押し出す
            if (overlap_left < overlap_right) {
              // 左に押し出す
              next_pos.x = block_left - player_half_width - 1.0f;  // 1px余裕を持たせる
            } else {
              // 右に押し出す
              next_pos.x = block_right + player_half_width + 1.0f;  // 1px余裕を持たせる
            }
            break;
          }
        }
      }
    }
  }

  player_->SetPosition(next_pos.x, next_pos.y);

  // 横移動がない場合は早期リターン
  if (move_input.x == 0.0f) {
    if (is_on_block) {
      player_->SetPose(Player::Pose::kIdle);
    } else {
      player_->SetPose(Player::Pose::kFall);
    }
    return;
  }

  // プレイヤーの現在位置を更新
  player_pos = player_->GetPosition();
  const float move_speed = player_->move_speed_;
  const float move_distance = move_speed * delta_time;

  // 次の位置を計算
  Vec2 horizontal_next_pos = player_pos;
  horizontal_next_pos.x += move_input.x * move_distance;

  // プレイヤーの左右端を計算
  const float player_left_h = horizontal_next_pos.x - player_half_width;
  const float player_right_h = horizontal_next_pos.x + player_half_width;
  const float player_top_h = horizontal_next_pos.y - player_->GetHeight() / 2.0f;
  const float player_bottom_h = horizontal_next_pos.y + player_->GetHeight() / 2.0f;

  // ブロックとの左右衝突判定
  bool can_move = true;

  for (size_t i = 0; i < block_grid_.size(); i++) {
    for (size_t j = 0; j < block_grid_[i].size(); j++) {
      const Block& block = block_grid_[i][j];

      // 空または破壊されたブロックはスキップ（壁は衝突対象）
      if (block.isEmpty() && !block.isWall()) {
        continue;
      }

      const Vec2 block_pos = block.position;
      const float block_left = block_pos.x;
      const float block_right = block_pos.x + InGameConstants::kBlockSize;
      const float block_top = block_pos.y;
      const float block_bottom = block_pos.y + InGameConstants::kBlockSize;

      // プレイヤーとブロックのY座標が重なっているかチェック
      const bool y_overlap = !(player_bottom_h <= block_top || player_top_h >= block_bottom);

      if (!y_overlap) {
        continue;
      }

      // 左に移動する場合
      if (move_input.x < 0) {
        player_->SetPose(Player::Pose::kStrafeLeft);

        // プレイヤーの左端がブロックの右端より左にあり、かつ衝突する場合
        if (player_left_h < block_right && player_right_h > block_right) {
          // ブロックの右端にプレイヤーの左端を配置
          horizontal_next_pos.x = block_right + player_half_width;
          can_move = false;
          break;
        }
      }
      // 右に移動する場合
      else if (move_input.x > 0) {
        player_->SetPose(Player::Pose::kStrafeRight);

        // プレイヤーの右端がブロックの左端より右にあり、かつ衝突する場合
        if (player_right_h > block_left && player_left_h < block_left) {
          // ブロックの左端にプレイヤーの右端を配置
          horizontal_next_pos.x = block_left - player_half_width;
          can_move = false;
          break;
        }
      }
    }

    if (!can_move) {
      break;
    }
  }

  // 画面端チェック
  if (horizontal_next_pos.x - player_half_width < 0) {
    horizontal_next_pos.x = player_half_width;
  } else if (horizontal_next_pos.x + player_half_width > InGameConstants::kScreenWidth) {
    horizontal_next_pos.x = InGameConstants::kScreenWidth - player_half_width;
  }

  // 位置を更新
  player_->SetPosition(horizontal_next_pos.x, player_pos.y);
}

void Game::update()
{
  // Esc キーでメニュー開閉
  if (KeyEscape.down())
  {
    PRINT << U"Toggle Menu";
    if (menu_->IsOpen())
    {
      menu_->Close();
      is_paused_ = false;
      PRINT << U"Close";
    }
    else
    {
      menu_->Open();
      is_paused_ = true;
      PRINT << U"Open";
    }
  }

  // メニューが開いている場合
  if (menu_->IsOpen())
  {
    if (!KeyEscape.down())
    {
      menu_->Update();

      // ゲーム終了がリクエストされたかチェック
      if (menu_->IsQuitRequested())
      {
        System::Exit();
        return;
      }

#if _DEBUG
      // ゲーム再起動がリクエストされたかチェック（DEBUGのみ）
      if (menu_->IsRestartRequested())
      {
        changeScene(EnumScene::kInGame, 0s);
        return;
      }
#endif
    }

    return;  // ゲームロジックは更新しない
  }

  // ゲームオーバー処理
  if (is_game_over_)
  {
    game_over_timer_ += Scene::DeltaTime();

    // 泡エフェクトの更新
    if (bubble_effect_)
    {
      bubble_effect_->Update(Scene::DeltaTime());
    }

    // 一定時間経過後にタイトルに戻る
    Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0, Min(0.7, game_over_timer_ * 0.5) });
    if (game_over_timer_ >= InGameConstants::kGameOverDuration)
    {
      TransitionToResult(false);
    }

    return;  // ゲームオーバー中は通常の更新処理をスキップ
  }

  // ゲームクリア処理
  if (is_game_clear_)
  {
    game_clear_timer_ += Scene::DeltaTime();

    // 一定時間経過後にタイトルに戻る
    if (game_clear_timer_ >= InGameConstants::kGameClearDuration)
    {
      TransitionToResult(true);
    }

    return;  // ゲームクリア中は通常の更新処理をスキップ
  }

  // ブロック破壊演出を更新
  if (block_destroy_effect_)
  {
    block_destroy_effect_->Update(Scene::DeltaTime());
  }
  
  // ヒット演出の更新
  hit_effect_.Update(Scene::DeltaTime());

  // have_words_を連結して1行で表示（互換性のため維持）
  String concatenated;
  for (const auto& word : have_words_) {
    concatenated += word;
  }

  // 単語が完成したかチェック（have_words_とcollected_characters_の両方で確認）
  Array<String> result = block_manager_.GetHitWords(have_words_, keywords);
  if (!result.isEmpty()) {
    // resultの各単語について処理
    for (const auto& hit_word : result) {
      // 完成した単語をcompleted_words_に追加（重複チェック）
      if (!completed_words_.includes(hit_word)) {
        completed_words_.push_back(hit_word);
        const auto messageIt = messages.find(hit_word);
        if (messageIt != messages.end() && !messageIt->second.isEmpty())
        {
          current_message_ = messageIt->second;
          message_timer_ = 0.0;
        }
        else {
          current_message_ = U"";
        }
        // HitEffect をプレイヤーの少し上に生成
        if (player_) {
          const Vec2 pos = player_->GetPosition() + Vec2{ 0.0, -60.0 };
          hit_effect_.Add(pos, hit_word);
        }
        
        // 単語を完成させたらエアを回復
        const size_t word_length = hit_word.length();
        air_amount_ += static_cast<float>(word_length) * InGameConstants::kAirRecoverRate;
        if (air_amount_ > InGameConstants::kAirMax) {
          air_amount_ = InGameConstants::kAirMax;
        }
        
        // 効果音など入れるならここ
        AudioManager::GetInstance()->PlaySe(SeKind::kCompleteWord);

        // クリア条件チェック
        if (completed_words_.size() >= static_cast<size_t>(InGameConstants::kClearEvaluationCount))
        {
          StartGameClear();
        }
      }
    }
  }

  // エア残量がゼロになったらゲームオーバー
  if (air_amount_ <= 0.0f && !is_game_over_)
  {
    StartGameOver();
  }

  // 時間経過によるエア減少（ポーズ中・ゲームオーバー・ゲームクリア時は減少しない）
  if (!is_paused_ && !is_game_over_ && !is_game_clear_)
  {
#if _DEBUG
    // デバッグモードでエア無限が有効な場合はエアを減少させない
    if (!menu_ || !menu_->IsInfiniteAirEnabled())
#endif
    {
      air_amount_ -= InGameConstants::kAirDecreaseRate * static_cast<float>(Scene::DeltaTime()) * static_cast<float>(std::sqrt(static_cast<float>(stage_)));
      if (air_amount_ < 0.0f) {
        air_amount_ = 0.0f;
      }
    }
  }

  // Zキーでブロック破壊
  if (KeyZ.down())
  {
    DestroyBlockUnderPlayer();
  }

  if (!is_paused_)
  {
    hint_timer_ += Scene::DeltaTime();
    if (hint_timer_ >= InGameConstants::kHintUpdateInterval)
    {
      hint_timer_ = 0.0;
      UpdateHint();
    }

    if (!current_message_.isEmpty())
    {
      message_timer_ += Scene::DeltaTime();
      if (message_timer_ >= InGameConstants::kMessageDisplayDuration)
      {
        current_message_.clear();
        message_timer_ = 0.0;
      }
    }
  }
  
  // UIの更新（メニューが閉じている時のみ）
  if (ui_)
  {
    ui_->Update(static_cast<float>(Scene::DeltaTime()));
    ui_->SetAirGauge(air_amount_);
  }

  // プレイヤーの左右移動更新（衝突判定付き）
  UpdatePlayerMovement(static_cast<float>(Scene::DeltaTime()));

  // スクロール更新（新しいブロック塊の生成判定）
  UpdateScroll();

  // プレイヤーの更新（メニューが閉じている時のみ）
  // 注：移動処理は上で行っているため、ここではアニメーションのみ更新
  if (player_)
  {
    player_->Update(static_cast<float>(Scene::DeltaTime()));
  }

  // カメラ位置を更新（プレイヤーに追従）
  UpdateCamera();
}

void Game::DrawDebugInfo() const
{
  if (!kDebugMode) {
    return;
  }

  // プレイヤーの位置を取得
  const Vec2 player_pos = player_->GetPosition();

  // プレイヤーのサイズを取得（スケール適用後）
  const float player_width = player_->GetWidth();
  const float player_height = player_->GetHeight();

  // プレイヤーの当たり判定を赤色の枠で描画
  const RectF player_hit_box{
    player_pos.x - player_width / 2.0f,
    player_pos.y - player_height / 2.0f,
    player_width,
    player_height
  };

  player_hit_box.drawFrame(InGameConstants::kDebugFrameThickness, InGameConstants::kDebugFrameColorRed);

  // プレイヤーの中心点を描画
  Circle{ player_pos, InGameConstants::kDebugCircleRadius }.draw(InGameConstants::kDebugCircleColorRed);

  // グリッド位置を取得
  int32 grid_row, grid_col;
  if (GetPlayerGridPosition(grid_row, grid_col)) {
    // 現在のグリッドを緑色の枠で描画
    const Vec2 grid_top_left = GetGridTopLeft(grid_row, grid_col);
    const RectF current_grid{
      grid_top_left.x,
      grid_top_left.y,
      InGameConstants::kBlockSize,
      InGameConstants::kBlockSize
    };
    //current_grid.drawFrame(3.0, Palette::Green);

    // グリッド座標を表示（メンバー変数のフォントを使用）
    debug_font_(U"Grid: ({}, {})"_fmt(grid_col, grid_row))
      .draw(InGameConstants::kDebugTextX, InGameConstants::kDebugTextY1, InGameConstants::kDebugTextColorWhite);
    debug_font_(U"Pos: ({:.1f}, {:.1f})"_fmt(player_pos.x, player_pos.y))
      .draw(InGameConstants::kDebugTextX, InGameConstants::kDebugTextY2, InGameConstants::kDebugTextColorWhite);
    debug_font_(U"Fall Velocity: {:.1f}"_fmt(player_fall_velocity_))
      .draw(InGameConstants::kDebugTextX, InGameConstants::kDebugTextY3, InGameConstants::kDebugTextColorWhite);
  }
}

void Game::draw() const
{
  Scene::SetBackground(InGameConstants::kBackgroundColor);

  if (!block_bg_texture_.isEmpty())
  {
    block_bg_texture_.resized(Scene::Size()).draw(0, 0);
  }

  // カメラオフセットを適用した変換を開始
  {
    const Transformer2D transformer{ Mat3x2::Translate(-camera_offset_) };

    // ブロックグリッドの描画
    const size_t texture_count = block_textures_.size();
    const bool has_block_textures = (texture_count > 0);
    const size_t color_count = InGameConstants::kBlockColors.size();

    for (size_t row = 0; row < block_grid_.size(); ++row) {
      for (size_t col = 0; col < block_grid_[row].size(); ++col) {
        const Block& block = block_grid_[row][col];

        // 空のブロックまたは破壊されたブロックはスキップ（壁は描画する）
        if (block.isEmpty() && !block.isWall()) {
          continue;
        }

        // ブロックの位置を直接使用（既にコンストラクタで設定済み）
        const Vec2 block_top_left = block.position;
        const Vec2 block_center = block_top_left + Vec2{ InGameConstants::kBlockSize / 2.0f, InGameConstants::kBlockSize / 2.0f };

        const RoundRect block_shape{ block_top_left.x, block_top_left.y, InGameConstants::kBlockSize, InGameConstants::kBlockSize, InGameConstants::kBlockRoundRadius };

        // 壁ブロックの場合
        if (block.isWall()) {
          block_shape.draw(InGameConstants::kWallBlockColor);
          block_shape.drawFrame(InGameConstants::kBlockFrameThickness, InGameConstants::kWallBlockFrameColor);

          // 壁のパターンを描画（斜線など）
          for (double offset = -InGameConstants::kBlockSize; offset < InGameConstants::kBlockSize * 2; offset += InGameConstants::kWallPatternLineSpacing) {
            const Vec2 start{ block_top_left.x + offset, block_top_left.y };
            const Vec2 end{ block_top_left.x + offset + InGameConstants::kBlockSize, block_top_left.y + InGameConstants::kBlockSize };
            Line{ start, end }.draw(InGameConstants::kWallPatternLineThickness, InGameConstants::kWallPatternColor);
          }
        }
        // 通常のブロックの場合
        else {
          // ブロックの見た目を位置依存のシードで決定
          const size_t seed = (row * InGameConstants::kSeedMultiplierRow + col * InGameConstants::kSeedMultiplierCol);

          if (has_block_textures) {
            const Texture& block_texture = block_textures_[seed % texture_count];
            const TextureRegion block_region = block_texture.resized(InGameConstants::kBlockSize, InGameConstants::kBlockSize);
            block_shape(block_region).draw();
          } else {
            block_shape.draw(block_grid_[row][col].color);
          }

          // ブロックの枠線を描画
          block_shape.drawFrame(InGameConstants::kBlockFrameThickness, InGameConstants::kBlockFrameColor);

          // ブロック内のテキストを中央に描画
          const Vec2 shadow_pos = block_center + InGameConstants::kBlockTextShadowOffset;
          block_font_(block.value).drawAt(shadow_pos.x, shadow_pos.y, InGameConstants::kBlockTextShadowColor);
          block_font_(block.value).drawAt(block_center.x, block_center.y, InGameConstants::kBlockTextColor);

          // プレイヤーに 1 ブロック以内かつ first の場合、?を小さく描画（斜め上）
          if (block.is_first && player_)
          {
            const Vec2 player_pos = player_->GetPosition();
            const double dx = std::abs(player_pos.x - block_center.x);
            const double dy = std::abs(player_pos.y - block_center.y);
            const bool within_one = (dx <= InGameConstants::kBlockSize && dy <= InGameConstants::kBlockSize);
            if (within_one && !sparkle_tex_.isEmpty())
            {
              // 位置（少し上に浮かせる）
              const Vec2 base_pos = block_top_left + Vec2{ InGameConstants::kBlockSize * 0.75f, InGameConstants::kBlockSize * 0.25f };

              // キラキラのパラメータ（時間で脈動＋回転）
              const double t = Scene::Time();
              const double pulse = 1.0 + 0.25 * Sin(t * 8.0);
              const double rot = t * 2.5;
              const double baseScale = 0.28; // 全体サイズを少し小さく
              const double scale = baseScale * pulse;
              const Vec2 pos = base_pos + Vec2{ 0.0, Sin(t * 4.0) * 2.0 }; // わずかに上下に揺れる

              // 加算合成で光彩を重ねて目立たせる
              const ScopedRenderStates2D addBlend{ BlendState::Additive };

              // 外側の淡い光（大きく、透明）
              sparkle_tex_.scaled(scale * 1.7).rotated(rot * 0.5)
                .drawAt(pos, ColorF{ 1.0, 0.95, 0.4, 0.18 });
              // 中間の光
              sparkle_tex_.scaled(scale * 1.3).rotated(-rot * 0.8)
                .drawAt(pos, ColorF{ 1.0, 0.95, 0.6, 0.28 });
              // 本体（白っぽく強い光）
              sparkle_tex_.scaled(scale).rotated(rot)
                .drawAt(pos, ColorF{ 1.0, 1.0, 1.0, 0.95 });
            }
          }
        }
      }
    }

    // ブロック破壊演出の描画（カメラオフセット適用範囲内）
    if (block_destroy_effect_)
    {
      block_destroy_effect_->Draw();
    }

    // 泡エフェクトの描画（ゲームオーバー演出）
    if (bubble_effect_ && is_game_over_)
    {
      bubble_effect_->Draw();
    }

    // プレイヤーの描画（カメラオフセット適用範囲内）
    // ゲームオーバー時は徐々に透明にする
    if (player_)
    {
      const Vec2 player_pos = player_->GetPosition();
      const auto texture = player_->GetTexture();

      if (texture)
      {
        const float scale_x = player_->GetScaleX();
        const float scale_y = player_->GetScaleY();
        
        if (is_game_over_)
        {
          // ゲームオーバー時はフェードアウト
          const double alpha = Max(0.0, 1.0 - (game_over_timer_ / (InGameConstants::kGameOverDuration * 0.5)));
          texture->scaled(scale_x, scale_y).drawAt(player_pos.x, player_pos.y, ColorF{ 1.0, 1.0, 1.0, alpha });
        }
        else if (is_game_clear_)
        {
          // ゲームクリア時は通常表示（フェードなし）
          texture->scaled(scale_x, scale_y).drawAt(player_pos.x, player_pos.y);
        }
        else
        {
          texture->scaled(scale_x, scale_y).drawAt(player_pos.x, player_pos.y);
        }
      }

      // ゲームオーバー時・ゲームクリア時は武器を非表示
      if (!is_game_over_ && !is_game_clear_ && player_->IsWeaponVisible())
      {
        const Vec2 weapon_pos = player_->GetWeaponPosition();
        const SizeF weapon_size = player_->GetWeaponSize();
        const double weapon_rotation = player_->GetWeaponRotation();
        const ColorF weapon_color = player_->GetWeaponColor();

        const Transformer2D weapon_transform{ Mat3x2::Rotate(weapon_rotation, weapon_pos), TransformCursor::No };
        if (weapon_texture_.isEmpty())
        {
          // テクスチャがない場合は従来の矩形
          RoundRect{ Arg::center(weapon_pos), weapon_size, InGameConstants::kWeaponRoundRadius }.draw(weapon_color);
        }
        else
        {
          // 武器の「長さ」（縦サイズ）は既存の高さを維持し、幅はテクスチャの縦横比から計算
          // pen.png は 1500x300（縦横比 5:1 の横長）。
          const double texW = weapon_texture_.width();
          const double texH = weapon_texture_.height();
          const double aspect = (texW > 0 && texH > 0) ? (texW / texH) : 5.0; // 1500/300=5

          // 既存の武器サイズ (w,h) のうち、長さとみなすのは height（縦方向）。
          const double targetH = weapon_size.y;
          const double targetW = targetH * aspect;

          const RectF weapon_rect{ Arg::center(weapon_pos), targetW, targetH };
          // テクスチャの本来の色で描画（色乗算しない）
          weapon_texture_.resized(weapon_rect.size).drawAt(weapon_pos);
        }
      }

      // ゲームオーバー時は吹き出し非表示
      if (!is_game_over_ && !is_game_clear_)
      {
        String bubble_text;
        if (!current_message_.isEmpty())
        {
          bubble_text = current_message_;
        }
        else if (!current_hint_.isEmpty())
        {
          bubble_text = current_hint_;
        }

        if (!bubble_text.isEmpty()) {
          const Vec2 hint_center = player_pos + InGameConstants::kHintBoxOffset;
          const RectF text_region = hint_font_(bubble_text).region();
          const RoundRect hint_rect{ Arg::center(hint_center), text_region.w + InGameConstants::kHintBoxPadding * 2, text_region.h + InGameConstants::kHintBoxPadding * 2, InGameConstants::kHintBoxRoundRadius };
          hint_rect.draw(InGameConstants::kHintBackgroundColor);
          hint_rect.drawFrame(InGameConstants::kHintBoxFrameThickness, InGameConstants::kHintBorderColor);
          hint_font_(bubble_text).drawAt(hint_center, InGameConstants::kHintTextColor);

          const Vec2 bubble_anchor = player_pos + InGameConstants::kHintBubbleAnchorOffset;
          Circle{ bubble_anchor, InGameConstants::kHintBubble1Radius }.draw(InGameConstants::kHintBackgroundColor).drawFrame(InGameConstants::kHintBubbleFrameThickness1, InGameConstants::kHintBorderColor);
          Circle{ bubble_anchor + InGameConstants::kHintBubble2Offset, InGameConstants::kHintBubble2Radius }.draw(InGameConstants::kHintBackgroundColor).drawFrame(InGameConstants::kHintBubbleFrameThickness2, InGameConstants::kHintBorderColor);
        }
      }
    }

    // デバッグ情報の描画（カメラオフセット適用範囲内）
    // DrawDebugInfo();

    // 最前面にヒット演出を描画
    hit_effect_.Draw();
  }
  // カメラオフセット適用範囲ここまで

  // UI の描画（カメラの影響を受けない、画面固定）
  if (ui_)
  {
    ui_->Render();
  }

  // 文字表示ボックスの描画（もじぴったん風UI）
  if (word_display_box_)
  {
    // CollectedCharactersから文字配列を取得（互換性のためhave_words_も設定）
    if (collected_characters_)
    {
      word_display_box_->SetWords(collected_characters_->GetCharacters());
    }
    else
    {
      word_display_box_->SetWords(have_words_);
    }
    word_display_box_->SetCompletedWords(completed_words_);
    word_display_box_->Draw(block_font_);
  }

  //------- 右側のボード：完成した単語を表示 - 画面固定
  // ボードの背景を描画
  const RoundRect board_area{ InGameConstants::kCompletedBoardX, InGameConstants::kCompletedBoardY, InGameConstants::kCompletedBoardWidth, InGameConstants::kCompletedBoardHeight, InGameConstants::kCompletedBoardRoundRadius };
  board_area.draw(InGameConstants::kCompletedBoardBackgroundColor);

  // 完成数を大きく薄く表示して背景にアクセント
  const auto overlay_text = U"{}"_fmt(completed_words_.size());
  if (!overlay_text.isEmpty()) {
    const Vec2 overlay_center{
      InGameConstants::kCompletedBoardX + InGameConstants::kCompletedBoardWidth / 2.0,
      InGameConstants::kCompletedBoardY + InGameConstants::kCompletedBoardHeight / 2.0
    };
    const double overlay_scale = InGameConstants::kCompletedBoardOverlayScale;
    const Transformer2D overlay_transform{ Mat3x2::Scale(overlay_scale, overlay_center) };
    block_font_(overlay_text).drawAt(overlay_center, InGameConstants::kCompletedBoardOverlayColor);
  }

  board_area.drawFrame(InGameConstants::kCompletedBoardFrameThickness, InGameConstants::kCompletedBoardBorderColor);

  // タイトルを描画
  block_font_(U"完成した単語").drawAt(InGameConstants::kCompletedBoardX + InGameConstants::kCompletedBoardWidth / 2, InGameConstants::kCompletedBoardY + InGameConstants::kCompletedBoardTitleOffsetY, InGameConstants::kCompletedBoardTitleColor);

  // 完成した単語を4列グリッドで配置
  const double column_width = InGameConstants::kCompletedBoardWidth / static_cast<double>(InGameConstants::kCompletedBoardColumns);
  const double row_start_y = InGameConstants::kCompletedBoardY + InGameConstants::kCompletedBoardContentStartY;
  const double row_height = InGameConstants::kCompletedBoardLineHeight;

  for (size_t index = 0; index < completed_words_.size(); ++index) {
    const size_t column = index % InGameConstants::kCompletedBoardColumns;
    const size_t row = index / InGameConstants::kCompletedBoardColumns;
    const double text_x = InGameConstants::kCompletedBoardX + column * column_width + InGameConstants::kCompletedBoardColumnPadding;
    const double text_y = row_start_y + row * row_height;
    completed_word_font_(completed_words_[index]).draw(text_x, text_y, InGameConstants::kCompletedWordTextColor);
  }

  // 完成した単語の数を表示
  debug_font_(U"完成数: {}"_fmt(completed_words_.size())).draw(InGameConstants::kCompletedBoardX + InGameConstants::kCompletedBoardCountOffsetX, InGameConstants::kCompletedBoardY + InGameConstants::kCompletedBoardHeight - InGameConstants::kCompletedBoardCountOffsetY, ColorF{ 1.0 });

  // ゲームオーバー時の表示
  if (is_game_over_)
  {
    // 半透明の黒背景
    Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, Min(0.7, game_over_timer_ * 0.5) });

    // ゲームオーバーテキスト（フェードイン）
    const double text_alpha = Min(1.0, game_over_timer_);
    ColorF text_color = InGameConstants::kGameOverTextColor;
    text_color.a = text_alpha;
    game_over_font_(U"GAME OVER").drawAt(Scene::Center(), text_color);
  }

  // ゲームクリア時の表示
  if (is_game_clear_)
  {
    // 半透明の白背景
    Scene::Rect().draw(ColorF{ 1.0, 1.0, 1.0, Min(0.7, game_clear_timer_ * 0.5) });

    // ゲームクリアテキスト（フェードイン）
    const double text_alpha = Min(1.0, game_clear_timer_);
    ColorF text_color = InGameConstants::kGameClearTextColor;
    text_color.a = text_alpha;
    game_over_font_(U"GAME CLEAR!").drawAt(Scene::Center(), text_color);
  }

  // 明るさ設定を適用
  GameSettings::GetInstance()->ApplyBrightness();

  // メニューとオプションを最前面に描画
  if (menu_->IsOpen())
  {
    menu_->Draw();
  }
}

void Game::UpdateCamera()
{
  // プレイヤーの位置を取得
  const Vec2 player_pos = player_->GetPosition();

  // 縦方向の見た目オフセット（ブロック2個分、主人公を画面内で高く見せる）
  const float y_bias = static_cast<float>(InGameConstants::kBlockSize * InGameConstants::kCameraVerticalBiasBlocks);

  // カメラの目標位置を計算（プレイヤーを画面中央より上に配置）
  const Vec2 target_camera_pos = Vec2{
    player_pos.x - Scene::Width() / 2.0f,
    player_pos.y - Scene::Height() / 2.0f + y_bias
  };

  // カメラ位置をスムーズに更新（線形補間）
  camera_offset_ += (target_camera_pos - camera_offset_) * InGameConstants::kCameraFollowSpeed;

  // カメラの移動範囲を制限（必要に応じて）
  // 例：左端より左には移動しない
  if (camera_offset_.x < 0) {
    camera_offset_.x = 0;
  }

  // 上端より上には移動しない（壁の最上部を考慮）
  if (camera_offset_.y < InGameConstants::kWallStartY) {
    camera_offset_.y = InGameConstants::kWallStartY;
  }

  // 右端の制限（ブロックグリッドのサイズに応じて）
  const float world_width = InGameConstants::kStartX + block_grid_[0].size() * InGameConstants::kBlockSize;
  const float max_camera_x = world_width - Scene::Width();
  if (camera_offset_.x > max_camera_x && max_camera_x > 0) {
    camera_offset_.x = max_camera_x;
  }

  // 下端の制限（ブロックグリッドのサイズに応じて）
  const float world_height = InGameConstants::kWallStartY + block_grid_.size() * InGameConstants::kBlockSize;
  const float max_camera_y = world_height - Scene::Height();
  if (camera_offset_.y > max_camera_y && max_camera_y > 0) {
    camera_offset_.y = max_camera_y;
  }
}

void Game::UpdateHint()
{
  const Array<std::pair<String, String>> reach_words = block_manager_.GetReachWords(have_words_, keywords);

  if (reach_words.isEmpty())
  {
    current_hint_.clear();
    return;
  }

  Array<String> candidates;
  candidates.reserve(reach_words.size());
  for (const auto& entry : reach_words)
  {
    if (entry.first.isEmpty())
    {
      continue;
    }
    if (!entry.second.isEmpty())
    {
      String masked = entry.first;
      for (char32& ch : masked)
      {
        if (ch == entry.second.front())
        {
          ch = U'〇';
        }
      }
      candidates << masked;
    }
    else
    {
      candidates << entry.first;
    }
  }

  if (candidates.isEmpty())
  {
    current_hint_.clear();
    return;
  }

  const size_t index = static_cast<size_t>(Random(0, static_cast<int32>(candidates.size() - 1)));
  current_hint_ = candidates[index];
}

void Game::StartGameOver()
{
  is_game_over_ = true;
  game_over_timer_ = 0.0;

  // プレイヤーのポーズをゲームオーバーに設定
  if (player_)
  {
    player_->SetPose(Player::Pose::kGameOver);
  }

  // プレイヤーの位置に泡エフェクトを発生
  if (player_ && bubble_effect_)
  {
    bubble_effect_->AddEffect(player_->GetPosition());
  }

  // ゲームオーバー効果音を再生
  AudioManager::GetInstance()->PlaySe(SeKind::kGameOver);
}

void Game::StartGameClear()
{
  is_game_clear_ = true;
  game_clear_timer_ = 0.0;

  // プレイヤーのポーズを待機に設定
  if (player_)
  {
    player_->SetPose(Player::Pose::kIdle);
  }

  // ゲームクリア効果音を再生（完成単語と同じ音を使用）
  AudioManager::GetInstance()->PlaySe(SeKind::kCompleteWord);
}

void Game::TransitionToResult(bool cleared)
{
  auto& shared = getData<SaveData>();
  shared.last_game_cleared_ = cleared;
  shared.last_completed_words_ = completed_words_;

  changeScene(EnumScene::kResult, 0s);
}

void Game::UpdateScroll()
{
  if (!player_)
  {
    return;
  }

  const Vec2 player_pos = player_->GetPosition();
  
  // プレイヤーがトリガー位置を超えたら次のブロック塊を生成
  if (player_pos.y >= next_chunk_trigger_y_)
  {
    GenerateNextBlockChunk();
  }
}

void Game::GenerateNextBlockChunk()
{
  ++stage_;

  // 新しいブロック塊を生成
  const Array<Array<std::pair<String, bool>>> new_string_grid = block_manager_.GenerateBlockGrid(
    InGameConstants::kGridRows,
    InGameConstants::kGridColumns,
    30,
    10,
    keywords,
    3,
    false
  );

  const int32 total_columns = InGameConstants::kGridColumns + InGameConstants::kWallThickness * 2;
  const int32 new_chunk_rows = static_cast<int32>(new_string_grid.size());
  
  // 新しいブロック塊の開始Y座標（前の塊から500px離す）
  const float new_chunk_start_y = current_chunk_bottom_y_ + kChunkSpacing;
  
  // 壁を含めた新しい行数を計算
  const int32 wall_rows_before_chunk = static_cast<int32>((new_chunk_start_y - InGameConstants::kWallStartY) / InGameConstants::kBlockSize);
  const int32 total_new_rows = wall_rows_before_chunk + new_chunk_rows;
  
  // グリッドを拡張
  const size_t old_size = block_grid_.size();
  if (static_cast<size_t>(total_new_rows) > old_size)
  {
    block_grid_.resize(total_new_rows);
    
    // 新しく追加された行を初期化
    for (size_t row = old_size; row < static_cast<size_t>(total_new_rows); ++row)
    {
      block_grid_[row].resize(total_columns);
      
      for (size_t col = 0; col < static_cast<size_t>(total_columns); ++col)
      {
        const bool is_wall_column = (col < InGameConstants::kWallThickness || 
                                     col >= static_cast<size_t>(InGameConstants::kGridColumns + InGameConstants::kWallThickness));
        
        if (is_wall_column)
        {
          // 壁ブロックを配置
          block_grid_[row][col] = Block(Block::Type::kWall);
          const float wall_x = InGameConstants::kStartX + static_cast<int32>(col) * InGameConstants::kBlockSize;
          const float wall_y = InGameConstants::kWallStartY + static_cast<int32>(row) * InGameConstants::kBlockSize;
          block_grid_[row][col].position = Vec2{ wall_x, wall_y };
        }
        else
        {
          // 新しいブロック塊の範囲内かチェック
          if (static_cast<int32>(row) >= wall_rows_before_chunk && 
              static_cast<int32>(row) < wall_rows_before_chunk + new_chunk_rows)
          {
            const size_t chunk_row = static_cast<size_t>(static_cast<int32>(row) - wall_rows_before_chunk);
            const size_t chunk_col = static_cast<size_t>(static_cast<int32>(col) - InGameConstants::kWallThickness);
            const auto& cell = new_string_grid[chunk_row][chunk_col];
            
            block_grid_[row][col] = Block(cell.first, cell.second);
            const float block_x = InGameConstants::kStartX + static_cast<int32>(col) * InGameConstants::kBlockSize;
            const float block_y = new_chunk_start_y + chunk_row * InGameConstants::kBlockSize;
            block_grid_[row][col].position = Vec2{ block_x, block_y };
          }
          else
          {
            // 空ブロック
            block_grid_[row][col] = Block();
            const float empty_x = InGameConstants::kStartX + static_cast<int32>(col) * InGameConstants::kBlockSize;
            const float empty_y = InGameConstants::kWallStartY + static_cast<int32>(row) * InGameConstants::kBlockSize;
            block_grid_[row][col].position = Vec2{ empty_x, empty_y };
          }
        }
      }
    }
  }
  
  // 現在のブロック塊の下端を更新
  current_chunk_bottom_y_ = new_chunk_start_y + new_chunk_rows * InGameConstants::kBlockSize;
  
  // 次のトリガー位置を更新
  next_chunk_trigger_y_ = current_chunk_bottom_y_ - kChunkSpacing;
}
