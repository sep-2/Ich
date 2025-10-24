#include "stdafx.h"

#include "InGame.h"

#include "System/Audio/AudioManager.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/GameSettings.h"
#include "System/System/BlockManager.h"
#include "Keywords.cpp"

namespace InGammeConstants {
  const Vec2 kStartBlock{ 200, 200 };
}

Game::Game(const InitData& init)
  : IScene{ init }
  , m_emoji{ U"🐥"_emoji }
  , menu_(std::make_unique<Menu>())
  , ui_(std::make_shared<Ui>())
  , player_(std::make_shared<Player>())
  , air_amount_(1.0f)
  , block_font_{ 40, Typeface::Bold }
  , debug_font_{ 16 }
{
  //PRINT << U"Game::Game()";

  for (const auto& emoji : emojis) {
    // 絵文字の画像から形状情報を作成する
    polygons << Emoji::CreateImage(emoji).alphaToPolygonsCentered().simplified(2.0);

    // 絵文字の画像からテクスチャを作成する
    textures << Texture{ Emoji{ emoji } };
  }

  index = Random(polygons.size() - 1);

  auto& data = getData<SaveData>();

  PRINT << data.click_count_;

  // UIの初期設定（1280x720対応）
  ui_->SetAirGaugePosition(900, 50);   // 画面左上にエアゲージを配置
  ui_->SetAirGauge(air_amount_);

  // サイドボックスを画面右下に配置（1280x720対応）
  ui_->SetSideBoxPosition(880, 120);  // 画面右下（720-300=420）
  ui_->SetSideBoxVisible(true);

  // ブロックグリッドを生成（10行x10列、バッチサイズ20）
  const Array<Array<String>> stringGrid = block_manager_.GenerateBlockGrid(10, 6, 20, keywords);

  // String配列をBlock配列に変換
  block_grid_.resize(stringGrid.size());
  for (size_t row = 0; row < stringGrid.size(); ++row) {
    block_grid_[row].resize(stringGrid[row].size());
    for (size_t col = 0; col < stringGrid[row].size(); ++col) {
      block_grid_[row][col] = Block(stringGrid[row][col]);
      const float gridX = InGammeConstants::kStartBlock.x + row * kBlockSize;
      const float gridY = InGammeConstants::kStartBlock.y + col * kBlockSize;
      //block_grid_[row][col].position = Vec2(static_cast<int32>(gridX), static_cast<int32>(gridY));
      block_grid_[row][col].position = GetGridTopLeft(row, col);
    }
  }

  // プレイヤーの初期設定（グリッドの一番上の中央に配置）
  const int32 initialCol = 5;  // 中央
  const int32 initialRow = 0;  // 一番上
  const Vec2 initialPos = GridToPixel(initialRow, initialCol);

  //player_->SetPosition(initialPos.x, initialPos.y);
  player_->SetPosition(100, 20);
  player_->SetMoveSpeed(200.0f);  // 移動速度を200ピクセル/秒に設定
}
Game::~Game()
{
  //PRINT << U"Game::~Game()";
}

bool Game::PixelToGrid(const Vec2& pixelPos, int32& gridRow, int32& gridCol) const
{
  // ピクセル座標からグリッド座標を計算
  const float relativeX = pixelPos.x - kStartX;
  const float relativeY = pixelPos.y - kStartY;

  gridCol = static_cast<int32>(relativeX / kBlockSize);
  gridRow = static_cast<int32>(relativeY / kBlockSize);

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
  const float pixelX = kStartX + gridCol * kBlockSize + kBlockSize / 2.0f;
  const float pixelY = kStartY + gridRow * kBlockSize + kBlockSize / 2.0f;
  return Vec2{ pixelX, pixelY };
}

Vec2 Game::GetGridTopLeft(int32 gridRow, int32 gridCol) const
{
  // グリッドの左上座標を取得
  const float pixelX = kStartX + gridCol * kBlockSize;
  const float pixelY = kStartY + gridRow * kBlockSize;
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
      
      // 空または破壊されたブロックはスキップ
      if (block.isEmpty()) {
        continue;
      }
      
      const Vec2 blockPos = block.position;
      const float blockLeft = blockPos.x;
      const float blockRight = blockPos.x + kBlockSize;
      const float blockTop = blockPos.y;
      const float blockBottom = blockPos.y + kBlockSize;
      
      bool canDestroy = false;
      String direction;
      
      // 下のブロック（足元）
      if (playerPos.x >= blockLeft && playerPos.x <= blockRight) {
        if (playerBottomY >= blockTop && playerBottomY <= blockTop + 10.0f) {
          canDestroy = true;
          direction = U"下";
        }
      }
      
      // 左のブロック
      if (playerLeft >= blockLeft && playerLeft <= blockRight) {
        if (playerPos.y >= blockTop && playerPos.y <= blockBottom) {
          canDestroy = true;
          direction = U"左";
        }
      }
      
      // 右のブロック
      if (playerRight >= blockLeft && playerRight <= blockRight) {
        if (playerPos.y >= blockTop && playerPos.y <= blockBottom) {
          canDestroy = true;
          direction = U"右";
        }
      }
      
      // 上のブロック（頭上）
      if (playerPos.x >= blockLeft && playerPos.x <= blockRight) {
        if (playerTop <= blockBottom && playerTop >= blockBottom - 10.0f) {
          canDestroy = true;
          direction = U"上";
        }
      }
      
      if (canDestroy) {
        // ブロックを破壊
        block.is_destroyed = true;
        PRINT << U"Block destroyed (" << direction << U") at row: " << i << U", col: " << j;
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

  //// プレイヤーの下のブロックをチェック
  //const int32 belowRow = gridRow + 1;
  //bool hasBlockBelow = false;

  //if (belowRow >= 0 && belowRow < static_cast<int32>(block_grid_.size()) &&
  //  gridCol >= 0 && gridCol < static_cast<int32>(block_grid_[belowRow].size())) {
  //  hasBlockBelow = !block_grid_[belowRow][gridCol].isEmpty();
  //}

  //// 下にブロックがない場合、または現在のブロックがない場合は落下
  //const bool shouldFall = (!hasBlockBelow || !isOnBlock) && belowRow < static_cast<int32>(block_grid_.size());

  //if (shouldFall) {
  //  player_fall_velocity_ += kGravity * delta_time;
  //  player_fall_velocity_ = Min(player_fall_velocity_, kMaxFallSpeed);

  //  Vec2 playerPos = player_->GetPosition();
  //  playerPos.y += player_fall_velocity_ * delta_time;

  //  // 落下中に次のブロックに到達する可能性をチェック
  //  bool foundBlock = false;

  //  // 現在の行から下方向にブロックを探す
  //  for (int32 checkRow = gridRow + 1; checkRow < static_cast<int32>(block_grid_.size()); ++checkRow) {
  //    if (HasBlockAt(checkRow, gridCol)) {
  //      // ブロックの上端の位置を取得
  //      const Vec2 blockTopLeft = GetGridTopLeft(checkRow, gridCol);

  //      // プレイヤーの下端がブロックの上端に到達または超えた場合
  //      const float playerBottom = playerPos.y + player_->GetHeight() / 2.0f;

  //      if (playerBottom >= blockTopLeft.y) {
  //        // ブロックの中心位置に配置
  //        playerPos = GridToPixel(checkRow, gridCol);
  //        player_fall_velocity_ = 0.0f;
  //        foundBlock = true;
  //        break;
  //      }
  //    }
  //  }

  //  player_->SetPosition(playerPos.x, playerPos.y);
  //} else {
  //  // ブロックがある場合は落下速度をリセット
  //  player_fall_velocity_ = 0.0f;

  //  // プレイヤーを現在のブロックの中心に調整
  //  if (isOnBlock) {
  //    Vec2 playerPos = player_->GetPosition();
  //    const Vec2 targetPos = GridToPixel(gridRow, gridCol);

  //    // Y座標がずれている場合は徐々に補正
  //    if (std::abs(playerPos.y - targetPos.y) > 1.0f) {
  //      playerPos.y = targetPos.y;
  //      player_->SetPosition(playerPos.x, playerPos.y);
  //    }
  //  }
  //}
  
  // プレイヤーの下のブロックをチェック
  const int32 belowRow = gridRow + 1;
  bool hasBlockBelow = false;
  
  if (belowRow >= 0 && belowRow < static_cast<int32>(block_grid_.size()) &&
      gridCol >= 0 && gridCol < static_cast<int32>(block_grid_[belowRow].size())) {
    hasBlockBelow = !block_grid_[belowRow][gridCol].isEmpty();
  }
  
  // 下にブロックがない場合は落下
  if (!hasBlockBelow && belowRow < static_cast<int32>(block_grid_.size())) {
    player_fall_velocity_ += kGravity * delta_time;
    player_fall_velocity_ = Min(player_fall_velocity_, kMaxFallSpeed);
    
    Vec2 playerPos = player_->GetPosition();
    playerPos.y += player_fall_velocity_ * delta_time;
    
    // 次のブロックの位置を計算
    const float nextBlockY = kStartY + belowRow * kBlockSize + kBlockSize / 2.0f;
    
    // ブロックの位置に到達したら停止
    bool landed = false;
    if (playerPos.y >= nextBlockY) {
      playerPos.y = nextBlockY;
      player_fall_velocity_ = 0.0f;
      landed = true;
    }
    
    player_->SetPosition(playerPos.x, playerPos.y);

    if (landed) {
      player_->RefreshPoseFromMovement();
    } else {
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

  // ブロックが存在するかチェック（空でない、かつ破壊されていない）
  return !block_grid_[gridRow][gridCol].isEmpty();
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
    return;
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
  const float gravity = 4.0f;
  Vec2 nextPos = playerPos;
  nextPos.y += gravity;

  const float playerBottomY = nextPos.y + player_->GetHeight() / 2.0f;
  bool isOnBlock = false;

  // 重力による落下とブロック衝突判定
  for (int i = 0; i < block_grid_.size(); i++) {
    for (int j = 0; j < block_grid_[i].size(); j++) {
      const Block& block = block_grid_[i][j];

      // 空または破壊されたブロックはスキップ
      if (block.isEmpty()) {
        continue;
      }

      const Vec2 blockPos = block.position;
      const float blockLeft = blockPos.x;
      const float blockRight = blockPos.x + kBlockSize;
      const float blockTop = blockPos.y;
      const float blockBottom = blockPos.y + kBlockSize;

      // デバッグ用の線描画
      if (kDebugMode) {
        Line{ blockLeft, blockTop, blockLeft, blockBottom }.draw(2.0, Palette::Blue);
        Line{ blockRight, blockTop, blockRight, blockBottom }.draw(2.0, Palette::Orange);
      }

      // プレイヤーの中心がブロックのX範囲内にあるかチェック
      if (nextPos.x >= blockLeft && nextPos.x <= blockRight) {
        // プレイヤーの下端がブロックの上面付近にあるかチェック
        if (playerBottomY >= blockTop && playerBottomY <= blockTop + gravity + 5.0f) {
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

      // 空または破壊されたブロックはスキップ
      if (block.isEmpty()) {
        continue;
      }

      const Vec2 blockPos = block.position;
      const float blockLeft = blockPos.x;
      const float blockRight = blockPos.x + kBlockSize;
      const float blockTop = blockPos.y;
      const float blockBottom = blockPos.y + kBlockSize;

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
  } else if (horizontalNextPos.x + playerHalfWidth > 1280) {
    horizontalNextPos.x = 1280 - playerHalfWidth;
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

  // Zキーでブロック破壊
  if (KeyZ.down()) {
    DestroyBlockUnderPlayer();
  }

  // UIの更新（メニューが閉じている時のみ）
  if (ui_) {
    ui_->Update(static_cast<float>(Scene::DeltaTime()));

    // デモ用：時間経過でエアが減少
    air_amount_ -= static_cast<float>(Scene::DeltaTime() * 0.1);  // 10秒で空になる
    if (air_amount_ < 0.0f) {
      air_amount_ = 0.0f;
    }

    // スペースキーでエア回復（デモ用）
    if (KeySpace.pressed()) {
      air_amount_ += static_cast<float>(Scene::DeltaTime() * 0.5);  // 2秒で満タン
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

  // 以下、通常のゲームロジック
  accumulatedTime += Scene::DeltaTime();
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

  playerHitBox.drawFrame(2.0, Palette::Red);

  // プレイヤーの中心点を描画
  Circle{ playerPos, 3 }.draw(Palette::Red);

  // グリッド位置を取得
  int32 gridRow, gridCol;
  if (GetPlayerGridPosition(gridRow, gridCol)) {
    // 現在のグリッドを緑色の枠で描画
    const Vec2 gridTopLeft = GetGridTopLeft(gridRow, gridCol);
    const RectF currentGrid{
      gridTopLeft.x,
      gridTopLeft.y,
      kBlockSize,
      kBlockSize
    };
    //currentGrid.drawFrame(3.0, Palette::Green);

    // グリッド座標を表示（メンバー変数のフォントを使用）
    debug_font_(U"Grid: ({}, {})"_fmt(gridCol, gridRow))
      .draw(20, 20, Palette::White);
    debug_font_(U"Pos: ({:.1f}, {:.1f})"_fmt(playerPos.x, playerPos.y))
      .draw(20, 40, Palette::White);
    debug_font_(U"Fall Velocity: {:.1f}"_fmt(player_fall_velocity_))
      .draw(20, 60, Palette::White);
  }
}

void Game::draw() const
{
  Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

  // ブロックグリッドの描画
  const int32 blockSize = kBlockSize;

  // カラフルな色のパレット
  const Array<ColorF> blockColors = {
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

  for (size_t row = 0; row < block_grid_.size(); ++row) {
    for (size_t col = 0; col < block_grid_[row].size(); ++col) {
      const Block& block = block_grid_[row][col];

      // 空のブロックまたは破壊されたブロックはスキップ
      if (block.isEmpty()) {
        continue;
      }

      // ブロックの位置をグリッド座標から取得
      const Vec2 blockTopLeft = GetGridTopLeft(static_cast<int32>(row), static_cast<int32>(col));
      const Vec2 blockCenter = GridToPixel(static_cast<int32>(row), static_cast<int32>(col));

      // ブロックの色をランダムに選択（位置に基づいた擬似ランダム）
      const size_t seed = (row * 982451653ULL + col * 1572869ULL) % blockColors.size();
      const ColorF blockColor = blockColors[seed];

      // 角丸の矩形を描画
      RoundRect{ blockTopLeft.x, blockTopLeft.y, blockSize, blockSize, 15 }.draw(blockColor);

      // ブロックの枠線を描画
      RoundRect{ blockTopLeft.x, blockTopLeft.y, blockSize, blockSize, 15 }.drawFrame(2, ColorF{ 0.2, 0.2, 0.2, 0.5 });

      // ブロック内のテキストを中央に描画
      block_font_(block.value).drawAt(blockCenter.x, blockCenter.y, ColorF{ 1.0 });
    }
  }

  // UIの描画（メニューより先に描画）
  if (ui_) {
    ui_->Render();
  }

  // プレイヤーの描画
  if (player_) {
    player_->Render();
  }

  // デバッグ情報の描画
  DrawDebugInfo();

  // メニューが開いている場合は描画
  if (menu_->IsOpen()) {
    menu_->Draw();
  }

  // 明るさ設定を適用
  GameSettings::GetInstance()->ApplyBrightness();
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
