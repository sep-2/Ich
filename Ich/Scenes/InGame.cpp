#include "stdafx.h"

#include "InGame.h"

#include "System/Audio/AudioManager.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/GameSettings.h"
#include "System/System/BlockManager.h"
#include "Keywords.hpp"

namespace InGameConstants {
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
  constexpr int32 kCompletedBoardX = 720;
  constexpr int32 kCompletedBoardY = 120;
  constexpr int32 kCompletedBoardWidth = 500;
  constexpr int32 kCompletedBoardHeight = 550;
  constexpr int32 kCompletedBoardLineHeight = 18;

  constexpr double kHintUpdateInterval = 3.0;

  // ブロック破壊判定パラメータ
  constexpr float kBlockDestroyVerticalThreshold = 10.0f;  // 上下のブロック破壊判定の閾値

  // 物理演算パラメータ
  constexpr float kSimpleGravity = 4.0f;          // 簡易重力（UpdatePlayerMovement用）
  constexpr float kGravityMargin = 5.0f;          // 重力適用時のマージン

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
  const Vec2 kHintBoxOffset{ -80.0, -70.0 }; // 20px 下へ
  const Vec2 kHintBubbleAnchorOffset{ -50.0, -30.0 }; // 20px 下へ
  const Vec2 kHintBubble2Offset{ 15.0, 15.0 };
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

  // エア量調整パラメータ
  constexpr float kAirDecreaseRate = 0.1f;        // エア減少率（10秒で空になる）
  constexpr float kAirIncreaseRate = 0.5f;        // エア回復率（2秒で満タン）

  // フォントサイズ
  constexpr int32 kBlockFontSize = 40;
  constexpr int32 kCompletedWordFontSize = 16;
  constexpr int32 kHintFontSize = 20;
  constexpr int32 kDebugFontSize = 16;

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
  /// ブロックカラー
  /// </summary>
  const Array<ColorF> kBlockColors = {
  ColorF{ 1.0, 0.3, 0.3 },  // 赤
  ColorF{ 0.3, 1.0, 0.3 },  // 緑
  ColorF{ 0.3, 0.3, 1.0 },  // 青
  ColorF{ 1.0, 1.0, 0.3 },  // 黄
  ColorF{ 1.0, 0.3, 1.0 },  // マゼンタ
  ColorF{ 0.3, 1.0, 1.0 },  // シアン
  ColorF{ 1.0, 0.6, 0.3 },  // オレンジ
  ColorF{ 0.6, 0.3, 1.0 },  // 紫
  ColorF{ 0.3, 1.0, 0.6 },  // 緑青
  ColorF{ 1.0, 0.8, 0.3 },  // 金色
  };

  // 壁ブロックの色
  const ColorF kWallBlockColor{ 0.3, 0.3, 0.3 };  // ダークグレー
  const ColorF kWallBlockFrameColor{ 0.15, 0.15, 0.15, 0.8 }; // より暗いグレー
  const ColorF kWallPatternColor{ 0.2, 0.2, 0.2, 0.3 }; // パターンの色
  constexpr double kWallPatternLineSpacing = 15.0; // 壁パターンの線間隔
  constexpr double kWallPatternLineThickness = 1.5; // 壁パターンの線の太さ
}

Game::Game(const InitData& init)
  : IScene{ init }
  , block_bg_texture_(InGameConstants::kBlockBgTexturePath)
  , menu_(std::make_unique<Menu>())
  , ui_(std::make_shared<Ui>())
  , player_(std::make_shared<Player>())
  , block_destroy_effect_(std::make_unique<BlockDestroyEffect>())
  , air_amount_(1.0f)
  , block_font_{ InGameConstants::kBlockFontSize, Typeface::Bold }
  , completed_word_font_{ InGameConstants::kCompletedWordFontSize }
  , hint_font_{ InGameConstants::kHintFontSize }
  , debug_font_{ InGameConstants::kDebugFontSize }
{
  //PRINT << U"Game::Game()";

  for (const auto& path : InGameConstants::kBlockTexturePaths) {
    Texture texture{ path };
    if (texture.isEmpty()) {
      PRINT << U"Failed to load block texture: " << path;
      continue;
    }
    block_textures_ << texture;
  }
  for (const auto& emoji : emojis) {
    // 絵文字の画像から形状情報を作成する
    polygons << Emoji::CreateImage(emoji).alphaToPolygonsCentered().simplified(2.0);

    // 絵文字の画像からテクスチャを作成する
    textures << Texture{ Emoji{ emoji } };
  }

  auto& data = getData<SaveData>();

  PRINT << data.click_count_;

  // UIの初期設定（1280x720対応）
  ui_->SetAirGaugePosition(InGameConstants::kAirGaugeX, InGameConstants::kAirGaugeY);
  ui_->SetAirGauge(air_amount_);

  // サイドボックスを画面右下に配置（1280x720対応）
  ui_->SetSideBoxPosition(InGameConstants::kSideBoxX, InGameConstants::kSideBoxY);
  ui_->SetSideBoxVisible(true);

  // ブロックグリッドを生成（10行x6列、バッチサイズ20）
  const Array<Array<String>> stringGrid = block_manager_.GenerateBlockGrid(
    InGameConstants::kGridRows,
    InGameConstants::kGridColumns,
    30,
    10,
    keywords,
    3
  );

  // String配列をBlock配列に変換（壁を含む拡張グリッドを作成）
  const int32 totalColumns = InGameConstants::kGridColumns + InGameConstants::kWallThickness * 2;
  
  // 壁ブロックの高さを計算（画面最上部から通常ブロック領域の下端まで）
  const int32 wallHeight = static_cast<int32>(std::ceil((InGameConstants::kStartY + stringGrid.size() * InGameConstants::kBlockSize - InGameConstants::kWallStartY) / static_cast<float>(InGameConstants::kBlockSize)));
  
  block_grid_.resize(wallHeight);

  for (size_t row = 0; row < wallHeight; ++row) {
    block_grid_[row].resize(totalColumns);

    for (size_t col = 0; col < totalColumns; ++col) {
      const bool isWallColumn = (col < InGameConstants::kWallThickness || col >= InGameConstants::kGridColumns + InGameConstants::kWallThickness);
      
      // 左右の壁（画面最上部から開始）
      if (isWallColumn) {
        block_grid_[row][col] = Block(Block::Type::kWall);
        // 壁ブロックは画面最上部から配置
        const float wallX = InGameConstants::kStartX + col * InGameConstants::kBlockSize;
        const float wallY = InGameConstants::kWallStartY + row * InGameConstants::kBlockSize;
        block_grid_[row][col].position = Vec2{ wallX, wallY };
      }
      // 通常のブロック領域
      else {
        // 通常ブロック領域の行インデックスを計算
        const int32 normalBlockRowOffset = static_cast<int32>((InGameConstants::kStartY - InGameConstants::kWallStartY) / InGameConstants::kBlockSize);
        
        if (row >= normalBlockRowOffset && row < normalBlockRowOffset + static_cast<int32>(stringGrid.size())) {
          const size_t actualRow = row - normalBlockRowOffset;
          const size_t actualCol = col - InGameConstants::kWallThickness;
          block_grid_[row][col] = Block(stringGrid[actualRow][actualCol]);
          // 通常ブロックも壁座標系で位置を設定
          const float blockX = InGameConstants::kStartX + col * InGameConstants::kBlockSize;
          const float blockY = InGameConstants::kStartY + actualRow * InGameConstants::kBlockSize;
          block_grid_[row][col].position = Vec2{ blockX, blockY };
        }
        else {
          // 通常ブロック領域外は空ブロック
          block_grid_[row][col] = Block();
          const float emptyX = InGameConstants::kStartX + col * InGameConstants::kBlockSize;
          const float emptyY = InGameConstants::kWallStartY + row * InGameConstants::kBlockSize;
          block_grid_[row][col].position = Vec2{ emptyX, emptyY };
        }
      }
    }
  }

  // プレイヤーの初期設定（グリッドの一番上の中央に配置）
  const int32 normalBlockRowOffset = static_cast<int32>((InGameConstants::kStartY - InGameConstants::kWallStartY) / InGameConstants::kBlockSize);
  const int32 initialCol = InGameConstants::kGridColumns / 2 + InGameConstants::kWallThickness;  // 中央（壁を考慮）
  const int32 initialRow = normalBlockRowOffset;  // 通常ブロック領域の一番上
  const Vec2 initialPos = Vec2{
    InGameConstants::kStartX + initialCol * InGameConstants::kBlockSize + InGameConstants::kBlockSize / 2.0f,
    InGameConstants::kWallStartY + initialRow * InGameConstants::kBlockSize + InGameConstants::kBlockSize / 2.0f
  };

  //player_->SetPosition(initialPos.x, initialPos.y);
  player_->SetPosition(InGameConstants::kPlayerInitialX, InGameConstants::kPlayerInitialY);
  player_->SetMoveSpeed(InGameConstants::kPlayerMoveSpeed);  // 移動速度を200ピクセル/秒に設定

  for (size_t i = 0; i < max_string_; i++) {
    have_words_.push_back(U"");
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
  const float relativeX = pixelPos.x - InGameConstants::kStartX;
  const float relativeY = pixelPos.y - InGameConstants::kWallStartY;

  gridCol = static_cast<int32>(relativeX / InGameConstants::kBlockSize);
  gridRow = static_cast<int32>(relativeY / InGameConstants::kBlockSize);

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
  const float pixelX = InGameConstants::kStartX + gridCol * InGameConstants::kBlockSize + InGameConstants::kBlockSize / 2.0f;
  const float pixelY = InGameConstants::kStartY + gridRow * InGameConstants::kBlockSize + InGameConstants::kBlockSize / 2.0f;
  return Vec2{ pixelX, pixelY };
}

Vec2 Game::GetGridTopLeft(int32 gridRow, int32 gridCol) const
{
  // グリッドの左上座標を取得
  const float pixelX = InGameConstants::kStartX + gridCol * InGameConstants::kBlockSize;
  const float pixelY = InGameConstants::kStartY + gridRow * InGameConstants::kBlockSize;
  return Vec2{ pixelX, pixelY };
}

bool Game::GetPlayerGridPosition(int32& gridRow, int32& gridCol) const
{
  const Vec2 playerPos = player_->GetPosition();
  return PixelToGrid(playerPos, gridRow, gridCol);
}

void Game::DestroyBlockUnderPlayer()
{
  const Vec2 playerPos = player_->GetPosition();
  const float playerBottomY = playerPos.y + player_->GetHeight() / 2.0f;
  const float playerLeft = playerPos.x - player_->GetWidth() / 2.0f;
  const float playerRight = playerPos.x + player_->GetWidth() / 2.0f;
  const float playerTop = playerPos.y - player_->GetHeight() / 2.0f;

  // プレイヤーの周囲のブロックを探す
  for (size_t i = 0; i < block_grid_.size(); i++) {
    for (size_t j = 0; j < block_grid_[i].size(); j++) {
      Block& block = block_grid_[i][j];

      // 空、破壊済み、または壁ブロックはスキップ
      if (block.isEmpty() || block.isWall()) {
        continue;
      }

      const Vec2 blockPos = block.position;
      const float blockLeft = blockPos.x;
      const float blockRight = blockPos.x + InGameConstants::kBlockSize;
      const float blockTop = blockPos.y;
      const float blockBottom = blockPos.y + InGameConstants::kBlockSize;

      bool canDestroy = false;
      String direction;

      // 下のブロック（足元）
      if (playerPos.x >= blockLeft && playerPos.x <= blockRight) {
        if (playerBottomY >= blockTop && playerBottomY <= blockTop + InGameConstants::kBlockDestroyVerticalThreshold) {
          if (!KeyLeft.pressed() && !KeyRight.pressed()) {
            canDestroy = true;
            direction = U"下";
          }
        }
      }

      // 左のブロック
      if (KeyLeft.pressed() && playerLeft >= blockLeft && playerLeft <= blockRight) {
        if (playerPos.y >= blockTop && playerPos.y <= blockBottom) {
          canDestroy = true;
          direction = U"左";
        }
      }

      // 右のブロック
      if (KeyRight.pressed() && playerRight >= blockLeft && playerRight <= blockRight) {
        if (playerPos.y >= blockTop && playerPos.y <= blockBottom) {
          canDestroy = true;
          direction = U"右";
        }
      }

      // 上のブロック（頭上）
      if (playerPos.x >= blockLeft && playerPos.x <= blockRight) {
        if (playerTop <= blockBottom && playerTop >= blockBottom - InGameConstants::kBlockDestroyVerticalThreshold) {
          canDestroy = true;
          direction = U"上";
        }
      }

      if (canDestroy) {
        // ブロックの中心位置を計算（block.positionから直接計算）
        const Vec2 blockCenter = blockPos + Vec2{ InGameConstants::kBlockSize / 2.0f, InGameConstants::kBlockSize / 2.0f };

        // ブロックの色を決定（位置依存のシード）
        const size_t seed = (i * InGameConstants::kSeedMultiplierRow + j * InGameConstants::kSeedMultiplierCol);
        const size_t colorCount = InGameConstants::kBlockColors.size();
        const ColorF blockColor = InGameConstants::kBlockColors[seed % colorCount];

        // 破壊演出を追加
        if (block_destroy_effect_) {
          block_destroy_effect_->AddEffect(blockCenter, blockColor, block.value);
        }

        // ブロックを破壊
        block.is_destroyed = true;
        PRINT << U"Block destroyed (" << direction << U") at row: " << i << U", col: " << j;

        // 文字を追加
        have_words_.push_back(block.value);

        // max_string_を超えたら先頭から削除
        while (have_words_.size() > max_string_) {
          have_words_.erase(have_words_.begin());
          PRINT << U"Removed oldest character. Current size: " << have_words_.size();
        }

        hint_timer_ = 0.0;
        UpdateHint();

        return;  // 1つだけ破壊して終了
      }
    }
  }

  PRINT << U"No block found to destroy near player";
}

void Game::UpdatePlayerFall(float delta_time)
{
  int32 gridRow, gridCol;
  if (!GetPlayerGridPosition(gridRow, gridCol)) {
    return;
  }

  //// プレイヤーの現在位置のブロックをチェック
  const bool isOnBlock = HasBlockAt(gridRow, gridCol);

  // プレイヤーの下のブロックをチェック
  const int32 belowRow = gridRow + 1;
  bool hasBlockBelow = false;

  if (belowRow >= 0 && belowRow < static_cast<int32>(block_grid_.size()) &&
    gridCol >= 0 && gridCol < static_cast<int32>(block_grid_[belowRow].size())) {
    hasBlockBelow = HasBlockAt(belowRow, gridCol);  // isEmpty()ではなくHasBlockAt()を使用
  }

  // 下にブロックがない場合は落下
  if (!hasBlockBelow && belowRow < static_cast<int32>(block_grid_.size())) {
    player_fall_velocity_ += InGameConstants::kGravity * delta_time;
    player_fall_velocity_ = Min(player_fall_velocity_, InGameConstants::kMaxFallSpeed);

    Vec2 playerPos = player_->GetPosition();
    playerPos.y += player_fall_velocity_ * delta_time;

    // 下にブロックがあるかどうかを、ピクセル座標ベースでも確認
    const float playerBottomY = playerPos.y + player_->GetHeight() / 2.0f;
    bool shouldLand = false;
    float landingY = playerPos.y;

    // 全ブロックをチェックして衝突判定
    for (size_t i = 0; i < block_grid_.size(); i++) {
      for (size_t j = 0; j < block_grid_[i].size(); j++) {
        const Block& block = block_grid_[i][j];
        
        // 空または破壊されたブロックはスキップ（壁は衝突対象）
        if (block.isEmpty() && !block.isWall()) {
          continue;
        }

        const Vec2 blockPos = block.position;
        const float blockLeft = blockPos.x;
        const float blockRight = blockPos.x + InGameConstants::kBlockSize;
        const float blockTop = blockPos.y;

        // プレイヤーの中心がブロックのX範囲内にあるかチェック
        if (playerPos.x >= blockLeft && playerPos.x <= blockRight) {
          // プレイヤーの下端がブロックの上面に到達または超えた場合
          if (playerBottomY >= blockTop && playerPos.y < blockTop) {
            shouldLand = true;
            landingY = blockTop - player_->GetHeight() / 2.0f;
            break;
          }
        }
      }
      if (shouldLand) {
        break;
      }
    }

    if (shouldLand) {
      playerPos.y = landingY;
      player_fall_velocity_ = 0.0f;
      player_->SetPosition(playerPos.x, playerPos.y);
      player_->RefreshPoseFromMovement();
    } else {
      player_->SetPosition(playerPos.x, playerPos.y);
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
  Vec2 moveInput = Vec2::Zero();
  bool isMoving = false;
  bool facingLeft = false;

  const bool walkForwardLeft = KeyUp.pressed() || KeyW.pressed();
  const bool walkForwardRight = KeyDown.pressed() || KeyS.pressed();

  // 上下入力は「その場で向きを変えるだけ」なので歩行アニメーションには移行させず、待機ポーズを使用する。
  if (walkForwardLeft || walkForwardRight) {
    player_->SetMoving(false);
    player_->SetPose(Player::Pose::kIdle);
  }

  if (KeyLeft.pressed() || KeyA.pressed()) {
    moveInput.x = -1.0f;
    isMoving = true;
    facingLeft = true;
  } else if (KeyRight.pressed() || KeyD.pressed()) {
    moveInput.x = 1.0f;
    isMoving = true;
    facingLeft = false;
  }

  // プレイヤーの移動状態と向きを更新
  player_->SetMoving(isMoving);
  if (isMoving) {
    player_->SetFacingLeft(facingLeft);
  }

  // プレイヤーの現在位置を取得
  Vec2 playerPos = player_->GetPosition();
  Vec2 nextPos = playerPos;
  nextPos.y += InGameConstants::kSimpleGravity;

  const float playerBottomY = nextPos.y + player_->GetHeight() / 2.0f;
  bool isOnBlock = false;

  // 重力による落下とブロック衝突判定
  for (int i = 0; i < block_grid_.size(); i++) {
    for (int j = 0; j < block_grid_[i].size(); j++) {
      const Block& block = block_grid_[i][j];

      // 空または破壊されたブロックはスキップ（壁は衝突対象）
      if (block.isEmpty() && !block.isWall()) {
        continue;
      }

      const Vec2 blockPos = block.position;
      const float blockLeft = blockPos.x;
      const float blockRight = blockPos.x + InGameConstants::kBlockSize;
      const float blockTop = blockPos.y;
      const float blockBottom = blockPos.y + InGameConstants::kBlockSize;

      // デバッグ用の線描画
      if (kDebugMode) {
        Line{ blockLeft, blockTop, blockLeft, blockBottom }.draw(InGameConstants::kDebugLineThickness, InGameConstants::kDebugLineColorBlue);
        Line{ blockRight, blockTop, blockRight, blockBottom }.draw(InGameConstants::kDebugLineThickness, InGameConstants::kDebugLineColorOrange);
      }

      // プレイヤーの中心がブロックのX範囲内にあるかチェック
      if (nextPos.x >= blockLeft && nextPos.x <= blockRight) {
        // プレイヤーの下端がブロックの上面付近にあるかチェック
        if (playerBottomY >= blockTop && playerBottomY <= blockTop + InGameConstants::kSimpleGravity + InGameConstants::kGravityMargin) {
          // プレイヤーをブロックの上に配置
          nextPos.y = blockTop - player_->GetHeight() / 2.0f;
          isOnBlock = true;
          break;
        }
      }
    }

    if (isOnBlock) {
      break;
    }
  }

  player_->SetPosition(nextPos.x, nextPos.y);

  // 横移動がない場合は早期リターン
  if (moveInput.x == 0.0f) {
    return;
  }

  // プレイヤーの現在位置を更新
  playerPos = player_->GetPosition();
  const float moveSpeed = player_->move_speed_;
  const float moveDistance = moveSpeed * delta_time;

  // 次の位置を計算
  Vec2 horizontalNextPos = playerPos;
  horizontalNextPos.x += moveInput.x * moveDistance;

  // プレイヤーの左右端を計算
  const float playerHalfWidth = player_->GetWidth() / 2.0f;
  const float playerLeft = horizontalNextPos.x - playerHalfWidth;
  const float playerRight = horizontalNextPos.x + playerHalfWidth;
  const float playerTop = horizontalNextPos.y - player_->GetHeight() / 2.0f;
  const float playerBottom = horizontalNextPos.y + player_->GetHeight() / 2.0f;

  // ブロックとの左右衝突判定
  bool canMove = true;

  for (size_t i = 0; i < block_grid_.size(); i++) {
    for (size_t j = 0; j < block_grid_[i].size(); j++) {
      const Block& block = block_grid_[i][j];

      // 空または破壊されたブロックはスキップ（壁は衝突対象）
      if (block.isEmpty() && !block.isWall()) {
        continue;
      }

      const Vec2 blockPos = block.position;
      const float blockLeft = blockPos.x;
      const float blockRight = blockPos.x + InGameConstants::kBlockSize;
      const float blockTop = blockPos.y;
      const float blockBottom = blockPos.y + InGameConstants::kBlockSize;

      // プレイヤーとブロックのY座標が重なっているかチェック
      const bool yOverlap = !(playerBottom <= blockTop || playerTop >= blockBottom);

      if (!yOverlap) {
        continue;
      }

      // 左に移動する場合
      if (moveInput.x < 0) {
        // プレイヤーの左端がブロックの右端より左にあり、かつ衝突する場合
        if (playerLeft < blockRight && playerRight > blockRight) {
          // ブロックの右端にプレイヤーの左端を配置
          horizontalNextPos.x = blockRight + playerHalfWidth;
          canMove = false;
          break;
        }
      }
      // 右に移動する場合
      else if (moveInput.x > 0) {
        // プレイヤーの右端がブロックの左端より右にあり、かつ衝突する場合
        if (playerRight > blockLeft && playerLeft < blockLeft) {
          // ブロックの左端にプレイヤーの右端を配置
          horizontalNextPos.x = blockLeft - playerHalfWidth;
          canMove = false;
          break;
        }
      }
    }

    if (!canMove) {
      break;
    }
  }

  // 画面端チェック
  if (horizontalNextPos.x - playerHalfWidth < 0) {
    horizontalNextPos.x = playerHalfWidth;
  } else if (horizontalNextPos.x + playerHalfWidth > InGameConstants::kScreenWidth) {
    horizontalNextPos.x = InGameConstants::kScreenWidth - playerHalfWidth;
  }

  // 位置を更新
  player_->SetPosition(horizontalNextPos.x, playerPos.y);
}

void Game::update()
{
  // Esc キーでメニュー開閉
  if (KeyEscape.down()) {
    PRINT << U"Toggle Menu";
    if (menu_->IsOpen()) {
      menu_->Close();
      is_paused_ = false;
      PRINT << U"Close";
    } else {
      menu_->Open();
      is_paused_ = true;
      PRINT << U"Open";
    }
  }

  // メニューが開いている場合
  if (menu_->IsOpen()) {
    PRINT << U"IsOpen now";
    if (!KeyEscape.down()) {
      menu_->Update();

      // ゲーム終了がリクエストされたかチェック
      if (menu_->IsQuitRequested()) {
        System::Exit();
        return;
      }
    }

    return;  // ゲームロジックは更新しない
  }

  // ブロック破壊演出を更新
  if (block_destroy_effect_) {
    block_destroy_effect_->Update(Scene::DeltaTime());
  }
  // ヒット演出の更新
  hit_effect_.Update(Scene::DeltaTime());

  // have_words_を連結して1行で表示
  String concatenated;
  for (const auto& word : have_words_) {
    concatenated += word;
  }
  PRINT << U"Concatenated: " << concatenated;

  // 単語が完成したかチェック
  Array<String> result = block_manager_.GetHitWords(have_words_, keywords);
  if (!result.isEmpty()) {
    // resultの各単語について処理
    for (const auto& hitWord : result) {
      // 完成した単語をcompleted_words_に追加（重複チェック）
      if (!completed_words_.includes(hitWord)) {
        completed_words_.push_back(hitWord);
        // HitEffect をプレイヤーの少し上に生成
        if (player_) {
          const Vec2 pos = player_->GetPosition() + Vec2{ 0.0, -60.0 };
          hit_effect_.Add(pos, hitWord);
        }
        // 効果音など入れるならここ
      }
    }
  }

  // Zキーでブロック破壊
  if (KeyZ.pressed()) {
    DestroyBlockUnderPlayer();
  }


  if (!is_paused_) {
    hint_timer_ += Scene::DeltaTime();
    if (hint_timer_ >= InGameConstants::kHintUpdateInterval) {
      hint_timer_ = 0.0;
      UpdateHint();
    }
  }
  // UIの更新（メニューが閉じている時のみ）
  if (ui_) {
    ui_->Update(static_cast<float>(Scene::DeltaTime()));

    // デモ用：時間経過でエアが減少
    air_amount_ -= static_cast<float>(Scene::DeltaTime() * InGameConstants::kAirDecreaseRate);
    if (air_amount_ < 0.0f) {
      air_amount_ = 0.0f;
    }

    // スペースキーでエア回復（デモ用）
    if (KeySpace.pressed()) {
      air_amount_ += static_cast<float>(Scene::DeltaTime() * InGameConstants::kAirIncreaseRate);
      if (air_amount_ > 1.0f) {
        air_amount_ = 1.0f;
      }
    }

    ui_->SetAirGauge(air_amount_);
  }

  // プレイヤーの落下更新
  UpdatePlayerFall(static_cast<float>(Scene::DeltaTime()));

  // プレイヤーの左右移動更新（衝突判定付き）
  UpdatePlayerMovement(static_cast<float>(Scene::DeltaTime()));

  // プレイヤーの更新（メニューが閉じている時のみ）
  // 注：移動処理は上で行っているため、ここではアニメーションのみ更新
  if (player_) {
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
  const Vec2 playerPos = player_->GetPosition();

  // プレイヤーのサイズを取得（スケール適用後）
  const float playerWidth = player_->GetWidth();
  const float playerHeight = player_->GetHeight();

  // プレイヤーの当たり判定を赤色の枠で描画
  const RectF playerHitBox{
    playerPos.x - playerWidth / 2.0f,
    playerPos.y - playerHeight / 2.0f,
    playerWidth,
    playerHeight
  };

  playerHitBox.drawFrame(InGameConstants::kDebugFrameThickness, InGameConstants::kDebugFrameColorRed);

  // プレイヤーの中心点を描画
  Circle{ playerPos, InGameConstants::kDebugCircleRadius }.draw(InGameConstants::kDebugCircleColorRed);

  // グリッド位置を取得
  int32 gridRow, gridCol;
  if (GetPlayerGridPosition(gridRow, gridCol)) {
    // 現在のグリッドを緑色の枠で描画
    const Vec2 gridTopLeft = GetGridTopLeft(gridRow, gridCol);
    const RectF currentGrid{
      gridTopLeft.x,
      gridTopLeft.y,
      InGameConstants::kBlockSize,
      InGameConstants::kBlockSize
    };
    //currentGrid.drawFrame(3.0, Palette::Green);

    // グリッド座標を表示（メンバー変数のフォントを使用）
    debug_font_(U"Grid: ({}, {})"_fmt(gridCol, gridRow))
      .draw(InGameConstants::kDebugTextX, InGameConstants::kDebugTextY1, InGameConstants::kDebugTextColorWhite);
    debug_font_(U"Pos: ({:.1f}, {:.1f})"_fmt(playerPos.x, playerPos.y))
      .draw(InGameConstants::kDebugTextX, InGameConstants::kDebugTextY2, InGameConstants::kDebugTextColorWhite);
    debug_font_(U"Fall Velocity: {:.1f}"_fmt(player_fall_velocity_))
      .draw(InGameConstants::kDebugTextX, InGameConstants::kDebugTextY3, InGameConstants::kDebugTextColorWhite);
  }
}

void Game::draw() const
{
  Scene::SetBackground(InGameConstants::kBackgroundColor);

  if (!block_bg_texture_.isEmpty()) {
    block_bg_texture_.resized(Scene::Size()).draw(0, 0);
  }

  // カメラオフセットを適用した変換を開始
  {
    const Transformer2D transformer{ Mat3x2::Translate(-camera_offset_) };

    // ブロックグリッドの描画
    const size_t textureCount = block_textures_.size();
    const bool hasBlockTextures = (textureCount > 0);
    const size_t colorCount = InGameConstants::kBlockColors.size();

    for (size_t row = 0; row < block_grid_.size(); ++row) {
      for (size_t col = 0; col < block_grid_[row].size(); ++col) {
        const Block& block = block_grid_[row][col];

        // 空のブロックまたは破壊されたブロックはスキップ（壁は描画する）
        if (block.isEmpty() && !block.isWall()) {
          continue;
        }

        // ブロックの位置を直接使用（既にコンストラクタで設定済み）
        const Vec2 blockTopLeft = block.position;
        const Vec2 blockCenter = blockTopLeft + Vec2{ InGameConstants::kBlockSize / 2.0f, InGameConstants::kBlockSize / 2.0f };

        const RoundRect blockShape{ blockTopLeft.x, blockTopLeft.y, InGameConstants::kBlockSize, InGameConstants::kBlockSize, InGameConstants::kBlockRoundRadius };

        // 壁ブロックの場合
        if (block.isWall()) {
          blockShape.draw(InGameConstants::kWallBlockColor);
          blockShape.drawFrame(InGameConstants::kBlockFrameThickness, InGameConstants::kWallBlockFrameColor);

          // 壁のパターンを描画（斜線など）
          for (double offset = -InGameConstants::kBlockSize; offset < InGameConstants::kBlockSize * 2; offset += InGameConstants::kWallPatternLineSpacing) {
            const Vec2 start{ blockTopLeft.x + offset, blockTopLeft.y };
            const Vec2 end{ blockTopLeft.x + offset + InGameConstants::kBlockSize, blockTopLeft.y + InGameConstants::kBlockSize };
            Line{ start, end }.draw(InGameConstants::kWallPatternLineThickness, InGameConstants::kWallPatternColor);
          }
        }
        // 通常のブロックの場合
        else {
          // ブロックの見た目を位置依存のシードで決定
          const size_t seed = (row * InGameConstants::kSeedMultiplierRow + col * InGameConstants::kSeedMultiplierCol);

          if (hasBlockTextures) {
            const Texture& blockTexture = block_textures_[seed % textureCount];
            const TextureRegion blockRegion = blockTexture.resized(InGameConstants::kBlockSize, InGameConstants::kBlockSize);
            blockShape(blockRegion).draw();
          } else {
            const ColorF blockColor = InGameConstants::kBlockColors[seed % colorCount];
            blockShape.draw(blockColor);
          }

          // ブロックの枠線を描画
          blockShape.drawFrame(InGameConstants::kBlockFrameThickness, InGameConstants::kBlockFrameColor);

          // ブロック内のテキストを中央に描画
          const Vec2 shadowPos = blockCenter + InGameConstants::kBlockTextShadowOffset;
          block_font_(block.value).drawAt(shadowPos.x, shadowPos.y, InGameConstants::kBlockTextShadowColor);
          block_font_(block.value).drawAt(blockCenter.x, blockCenter.y, InGameConstants::kBlockTextColor);
        }
      }
    }

    // ブロック破壊演出の描画（カメラオフセット適用範囲内）
    if (block_destroy_effect_) {
      block_destroy_effect_->Draw();
    }

    // プレイヤーの描画（カメラオフセット適用範囲内）
    // Rendererシステムを使わずに直接描画してカメラに追従させる
    if (player_) {
      const Vec2 playerPos = player_->GetPosition();
      const auto texture = player_->GetTexture();

      if (texture) {
        const float scaleX = player_->GetScaleX();
        const float scaleY = player_->GetScaleY();
        texture->scaled(scaleX, scaleY).drawAt(playerPos.x, playerPos.y);
      }

      if (player_->IsWeaponVisible()) {
        const Vec2 weaponPos = player_->GetWeaponPosition();
        const SizeF weaponSize = player_->GetWeaponSize();
        const double weaponRotation = player_->GetWeaponRotation();
        const ColorF weaponColor = player_->GetWeaponColor();

        const Transformer2D weaponTransform{ Mat3x2::Rotate(weaponRotation, weaponPos), TransformCursor::No };
        RoundRect{ Arg::center(weaponPos), weaponSize, InGameConstants::kWeaponRoundRadius }.draw(weaponColor);
      }

      if (!current_hint_.isEmpty()) {
        const Vec2 hintCenter = playerPos + InGameConstants::kHintBoxOffset;
        const RectF textRegion = hint_font_(current_hint_).region();
        const RoundRect hintRect{ Arg::center(hintCenter), textRegion.w + InGameConstants::kHintBoxPadding * 2, textRegion.h + InGameConstants::kHintBoxPadding * 2, InGameConstants::kHintBoxRoundRadius };
        hintRect.draw(InGameConstants::kHintBackgroundColor);
        hintRect.drawFrame(InGameConstants::kHintBoxFrameThickness, InGameConstants::kHintBorderColor);
        hint_font_(current_hint_).drawAt(hintCenter, InGameConstants::kHintTextColor);

        const Vec2 bubbleAnchor = playerPos + InGameConstants::kHintBubbleAnchorOffset;
        Circle{ bubbleAnchor, InGameConstants::kHintBubble1Radius }.draw(InGameConstants::kHintBackgroundColor).drawFrame(InGameConstants::kHintBubbleFrameThickness1, InGameConstants::kHintBorderColor);
        Circle{ bubbleAnchor + InGameConstants::kHintBubble2Offset, InGameConstants::kHintBubble2Radius }.draw(InGameConstants::kHintBackgroundColor).drawFrame(InGameConstants::kHintBubbleFrameThickness2, InGameConstants::kHintBorderColor);
      }
    }

    // デバッグ情報の描画（カメラオフセット適用範囲内）
    DrawDebugInfo();

    // 最前面にヒット演出を描画
    hit_effect_.Draw();
  }
  // カメラオフセット適用範囲ここまで

  // UI の描画（カメラの影響を受けない、画面固定）
  if (ui_) {
    ui_->Render();
  }

  // メニューが開いている場合は描画
  if (menu_->IsOpen()) {
    menu_->Draw();
  }

  // 明るさ設定を適用
  GameSettings::GetInstance()->ApplyBrightness();

  //------- 文字表示（上部：現在収集中の文字）- もじぴったん風のボックス表示
  Array<String> result = block_manager_.GetHitWords(have_words_, keywords);

  for (int i = 0; i < have_words_.size(); i++) {
    const String& word = have_words_[i];

    // この文字が完成した単語に含まれているかチェック
    bool isInCompletedWord = false;

    for (const auto& completedWord : completed_words_) {
      if (completedWord.includes(word)) {
        isInCompletedWord = true;
        break;
      }
    }

    // ボックスの位置を計算
    const int32 boxX = InGameConstants::kCharBoxStartX + i * (InGameConstants::kCharBoxSize + InGameConstants::kCharBoxSpacing);
    const int32 boxY = InGameConstants::kCharBoxStartY;

    // ボックスの背景色（完成した単語に含まれる場合は明るい赤、それ以外は白）
    const ColorF boxColor = isInCompletedWord ? InGameConstants::kCharBoxCompletedBoxColor : InGameConstants::kCharBoxDefaultBoxColor;
    const ColorF borderColor = isInCompletedWord ? InGameConstants::kCharBoxCompletedBorderColor : InGameConstants::kCharBoxDefaultBorderColor;

    // ボックスを描画（角丸四角形）
    RoundRect{ boxX, boxY, InGameConstants::kCharBoxSize, InGameConstants::kCharBoxSize, InGameConstants::kCharBoxRoundRadius }.draw(boxColor);
    RoundRect{ boxX, boxY, InGameConstants::kCharBoxSize, InGameConstants::kCharBoxSize, InGameConstants::kCharBoxRoundRadius }.drawFrame(InGameConstants::kCharBoxFrameThickness, borderColor);

    // 文字を中央に描画（影付き）
    const Vec2 textCenter{ boxX + InGameConstants::kCharBoxSize / 2.0, boxY + InGameConstants::kCharBoxSize / 2.0 };
    constexpr Vec2 shadowOffset{ 2.0, 2.0 };

    // 影
    block_font_(word).drawAt(textCenter + InGameConstants::kCharBoxTextShadowOffset, InGameConstants::kCharBoxTextShadowColor);
    // 文字本体（完成した単語に含まれる場合は赤、それ以外は黒）
    const ColorF textColor = isInCompletedWord ? InGameConstants::kCharBoxCompletedTextColor : InGameConstants::kCharBoxDefaultTextColor;
    block_font_(word).drawAt(textCenter, textColor);
  }

  //------- 右側のボード：完成した単語を表示 - 画面固定
  // ボードの背景を描画
  RoundRect{ InGameConstants::kCompletedBoardX, InGameConstants::kCompletedBoardY, InGameConstants::kCompletedBoardWidth, InGameConstants::kCompletedBoardHeight, InGameConstants::kCompletedBoardRoundRadius }.draw(InGameConstants::kCompletedBoardBackgroundColor);
  RoundRect{ InGameConstants::kCompletedBoardX, InGameConstants::kCompletedBoardY, InGameConstants::kCompletedBoardWidth, InGameConstants::kCompletedBoardHeight, InGameConstants::kCompletedBoardRoundRadius }.drawFrame(InGameConstants::kCompletedBoardFrameThickness, InGameConstants::kCompletedBoardBorderColor);

  // タイトルを描画
  block_font_(U"完成した単語").drawAt(InGameConstants::kCompletedBoardX + InGameConstants::kCompletedBoardWidth / 2, InGameConstants::kCompletedBoardY + InGameConstants::kCompletedBoardTitleOffsetY, InGameConstants::kCompletedBoardTitleColor);

  // 完成した単語を4列グリッドで配置
  const double columnWidth = InGameConstants::kCompletedBoardWidth / static_cast<double>(InGameConstants::kCompletedBoardColumns);
  const double rowStartY = InGameConstants::kCompletedBoardY + InGameConstants::kCompletedBoardContentStartY;
  const double rowHeight = InGameConstants::kCompletedBoardLineHeight;

  for (size_t index = 0; index < completed_words_.size(); ++index) {
    const size_t column = index % InGameConstants::kCompletedBoardColumns;
    const size_t row = index / InGameConstants::kCompletedBoardColumns;
    const double textX = InGameConstants::kCompletedBoardX + column * columnWidth + InGameConstants::kCompletedBoardColumnPadding;
    const double textY = rowStartY + row * rowHeight;
    completed_word_font_(completed_words_[index]).draw(textX, textY, InGameConstants::kCompletedWordTextColor);
  }

  // 完成した単語の数を表示
  debug_font_(U"完成数: {}"_fmt(completed_words_.size())).draw(InGameConstants::kCompletedBoardX + InGameConstants::kCompletedBoardCountOffsetX, InGameConstants::kCompletedBoardY + InGameConstants::kCompletedBoardHeight - InGameConstants::kCompletedBoardCountOffsetY, ColorF{ 1.0 });
}

void Game::drawFadeIn(double t) const
{
  //draw();

  //// 1280x720対応のフェードイン効果
  //for (int32 y = 0; y < 8; ++y) {
  //  RectF{ (1280 + y * 120 - (1 + t) * 2560), (y * 90), 2560, 90 }.draw(HSV{ (y * 20), 0.2, 1.0 });
  //}
}

void Game::drawFadeOut(double t) const
{
  draw();

  // 1280x720対応のフェードアウト効果
  Circle{ 640, 360, 640 }
  .drawFrame((t * 640), 0, ColorF{ 0.2, 0.3, 0.4 });
}

void Game::UpdateCamera()
{
  // プレイヤーの位置を取得
  const Vec2 playerPos = player_->GetPosition();

  // 縦方向の見た目オフセット（ブロック2個分、主人公を画面内で高く見せる）
  const float yBias = static_cast<float>(InGameConstants::kBlockSize * InGameConstants::kCameraVerticalBiasBlocks);

  // カメラの目標位置を計算（プレイヤーを画面中央より上に配置）
  const Vec2 targetCameraPos = Vec2{
    playerPos.x - Scene::Width() / 2.0f,
    playerPos.y - Scene::Height() / 2.0f + yBias
  };

  // カメラ位置をスムーズに更新（線形補間）
  camera_offset_ += (targetCameraPos - camera_offset_) * InGameConstants::kCameraFollowSpeed;

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
  const float worldWidth = InGameConstants::kStartX + block_grid_[0].size() * InGameConstants::kBlockSize;
  const float maxCameraX = worldWidth - Scene::Width();
  if (camera_offset_.x > maxCameraX && maxCameraX > 0) {
    camera_offset_.x = maxCameraX;
  }

  // 下端の制限（ブロックグリッドのサイズに応じて）
  const float worldHeight = InGameConstants::kWallStartY + block_grid_.size() * InGameConstants::kBlockSize;
  const float maxCameraY = worldHeight - Scene::Height();
  if (camera_offset_.y > maxCameraY && maxCameraY > 0) {
    camera_offset_.y = maxCameraY;
  }
}

void Game::UpdateHint()
{
  const Array<std::pair<String, String>> reachWords = block_manager_.GetReachWords(have_words_, keywords);

  if (reachWords.isEmpty()) {
    current_hint_.clear();
    return;
  }

  Array<String> candidates;
  candidates.reserve(reachWords.size());
  for (const auto& entry : reachWords) {
    if (entry.first.isEmpty()) {
      continue;
    }
    if (!entry.second.isEmpty()) {
      String masked = entry.first;
      for (char32& ch : masked) {
        if (ch == entry.second.front()) {
          ch = U'〇';
        }
      }
      candidates << masked;
    } else {
      candidates << entry.first;
    }
  }

  if (candidates.isEmpty()) {
    current_hint_.clear();
    return;
  }

  const size_t index = static_cast<size_t>(Random(0, static_cast<int32>(candidates.size() - 1)));
  current_hint_ = candidates[index];
}
