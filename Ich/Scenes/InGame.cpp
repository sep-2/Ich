#include "stdafx.h"

#include "InGame.h"

#include "System/Audio/AudioManager.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/GameSettings.h"
#include "System/System/BlockManager.h"


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

  // 辞書の初期化（サンプル辞書）
  dictionary_ = {
    U"あい", U"うえお", U"かきく", U"けこ", U"さしす", U"せそ",
    U"たちつ", U"てと", U"なにぬ", U"ねの", U"はひふ", U"へほ",
    U"まみむ", U"めも", U"やゆよ", U"らりる", U"れろ", U"わをん"
  };

  // ブロックグリッドを生成（10行x20列、バッチサイズ20）
  block_grid_ = block_manager_.GenerateBlockGrid(10, 10, 20, dictionary_);

  // プレイヤーの初期設定（グリッドの一番上の中央に配置）
  const int32 initialCol = 5;  // 中央
  const int32 initialRow = 0;  // 一番上
  const float playerX = kStartX + initialCol * kBlockSize + kBlockSize / 2.0f;
  const float playerY = kStartY + initialRow * kBlockSize + kBlockSize / 2.0f;
  
  player_->SetPosition(100, 10);
  player_->SetMoveSpeed(200.0f);  // 移動速度を200ピクセル/秒に設定
}

Game::~Game()
{
  //PRINT << U"Game::~Game()";
}

bool Game::GetPlayerGridPosition(int32& gridRow, int32& gridCol) const
{
  const Vec2 playerPos = player_->GetPosition();
  
  // プレイヤーの中心位置からグリッド座標を計算
  const float relativeX = playerPos.x - kStartX;
  const float relativeY = playerPos.y - kStartY;
  
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

void Game::DestroyBlockUnderPlayer()
{
  int32 gridRow, gridCol;
  if (!GetPlayerGridPosition(gridRow, gridCol)) {
    return;
  }
  
  // プレイヤーの足元のブロック（1つ下）を破壊
  const int32 belowRow = gridRow + 1;
  
  if (belowRow >= 0 && belowRow < static_cast<int32>(block_grid_.size()) &&
      gridCol >= 0 && gridCol < static_cast<int32>(block_grid_[belowRow].size())) {
    
    // ブロックが存在する場合のみ破壊
    if (!block_grid_[belowRow][gridCol].isEmpty()) {
      block_grid_[belowRow][gridCol] = U"";  // ブロックを空にする
      PRINT << U"Block destroyed at row: " << belowRow << U", col: " << gridCol;
    }
  }
}

void Game::UpdatePlayerFall(float delta_time)
{
  int32 gridRow, gridCol;
  if (!GetPlayerGridPosition(gridRow, gridCol)) {
    return;
  }
  
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

  // ブロックが存在するかチェック
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

  if (walkForwardLeft || walkForwardRight) {
    player_->SetMoving(false);
    player_->SetPose(walkForwardLeft ? Player::Pose::kWalkForwardLeft : Player::Pose::kWalkForwardRight);
    return;
  }
  
  if (KeyLeft.pressed() || KeyA.pressed()) {
    moveInput.x = -1.0f;
    isMoving = true;
    facingLeft = true;
  }
  else if (KeyRight.pressed() || KeyD.pressed()) {
    moveInput.x = 1.0f;
    isMoving = true;
    facingLeft = false;
  }

  // プレイヤーの移動状態と向きを更新
  player_->SetMoving(isMoving);
  if (isMoving) {
    player_->SetFacingLeft(facingLeft);
  }

  // 移動がない場合は早期リターン
  if (moveInput.x == 0.0f) {
    return;
  }

  // プレイヤーの現在位置とグリッド座標を取得
  int32 currentRow, currentCol;
  if (!GetPlayerGridPosition(currentRow, currentCol)) {
    return;
  }

  Vec2 playerPos = player_->GetPosition();
  const float moveSpeed = 200.0f;  // ピクセル/秒
  const float moveDistance = moveSpeed * delta_time;

  // 次の位置を計算
  Vec2 nextPos = playerPos;
  nextPos.x += moveInput.x * moveDistance;

  // 左右のブロック衝突判定
  bool canMove = true;

  // 左に移動する場合
  if (moveInput.x < 0) {
    const int32 leftCol = currentCol - 1;
    
    // 現在のグリッドの中心位置
    const float currentGridCenterX = kStartX + currentCol * kBlockSize + kBlockSize / 2.0f;
    // 左のグリッドとの境界（2つのグリッドの中心の中間点）
    const float leftBoundary = currentGridCenterX - kBlockSize / 2.0f;
    
    // 次の位置が左の境界を超える場合
    if (nextPos.x < leftBoundary) {
      // 左隣のブロックが存在するかチェック
      if (HasBlockAt(currentRow, leftCol)) {
        canMove = false;
        // ブロックの境界ぴったりに位置を固定
        nextPos.x = leftBoundary;
      }
    }
  }
  // 右に移動する場合
  else if (moveInput.x > 0) {
    const int32 rightCol = currentCol + 1;
    
    // 現在のグリッドの中心位置
    const float currentGridCenterX = kStartX + currentCol * kBlockSize + kBlockSize / 2.0f;
    // 右のグリッドとの境界（2つのグリッドの中心の中間点）
    const float rightBoundary = currentGridCenterX + kBlockSize / 2.0f;
    
    // 次の位置が右の境界を超える場合
    if (nextPos.x > rightBoundary) {
      // 右隣のブロックが存在するかチェック
      if (HasBlockAt(currentRow, rightCol)) {
        canMove = false;
        // ブロックの境界ぴったりに位置を固定
        nextPos.x = rightBoundary;
      }
    }
  }

  // グリッドの範囲外チェック
  const int32 nextCol = static_cast<int32>((nextPos.x - kStartX) / kBlockSize);
  if (nextCol < 0) {
    canMove = false;
    nextPos.x = kStartX + kBlockSize / 2.0f;  // 左端の中心
  } else if (nextCol >= static_cast<int32>(block_grid_[0].size())) {
    canMove = false;
    nextPos.x = kStartX + (block_grid_[0].size() - 1) * kBlockSize + kBlockSize / 2.0f;  // 右端の中心
  }

  // 位置を更新（衝突していても境界までは移動する）
  player_->SetPosition(nextPos.x, playerPos.y);
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
    }
    else {
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

  //while (StepTime <= accumulatedTime) {
  //  // 2D 物理演算のワールドを更新する
  //  world.update(StepTime);

  //  accumulatedTime -= StepTime;
  //}

  //// 地面より下に落ちた物体は削除する
  //for (auto it = bodies.begin(); it != bodies.end();) {
  //  if (100 < it->getPos().y) {
  //    // 対応テーブルからも削除
  //    table.erase(it->id());

  //    it = bodies.erase(it);

  //    AudioManager::GetInstance()->PlaySe(SeKind::kDecideSe);
  //    changeScene(EnumScene::kTitle);
  //  } else {
  //    ++it;
  //  }
  //}

  //// 2D カメラを更新する
  //camera.update();
  //{
  //  // 2D カメラから Transformer2D を作成する
  //  const auto t = camera.createTransformer();

  //  // 左クリックされたら
  //  if (MouseL.down()) {
  //    // ボディを追加する
  //    bodies << world.createPolygons(P2Dynamic, Cursor::PosF(), polygons[index], P2Material{ 0.1, 0.0, 1.0 });

  //    // ボディ ID と絵文字のインデックスの組を対応テーブルに追加する
  //    table.emplace(bodies.back().id(), std::exchange(index, Random(polygons.size() - 1)));

  //    auto& data = getData<SaveData>();
  //    data.click_count_++;

  //    // メインループの後、終了時にゲームをセーブ
  //    {
  //      // バイナリファイルをオープン
  //      Serializer<BinaryWriter> writer{ U"game.save" };

  //      // シリアライズに対応したデータを書き出す
  //      writer(data);
  //    }
  //  }

  //  // すべてのボディを描画する
  //  for (const auto& body : bodies) {
  //    textures[table[body.id()]].rotated(body.getAngle()).drawAt(body.getPos());
  //  }

    // 地面を描画する
    ground.draw(Palette::Green);

  //  // 現在操作できる絵文字を描画する
  //  textures[index].drawAt(Cursor::PosF(), ColorF{ 1.0, (0.5 + Periodic::Sine0_1(1s) * 0.5) });
  //}

  //// 2D カメラの操作を描画する
  //camera.draw(Palette::Orange);
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
    const RectF currentGrid{
      kStartX + gridCol * kBlockSize,
      kStartY + gridRow * kBlockSize,
      kBlockSize,
      kBlockSize
    };
    currentGrid.drawFrame(3.0, Palette::Green);
    
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
  const int32 startX = kStartX;
  const int32 startY = kStartY;

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
      const String& blockText = block_grid_[row][col];
      
      if (blockText.isEmpty()) {
        continue;
      }

      // ブロックの位置を計算
      const int32 x = startX + static_cast<int32>(col) * blockSize;
      const int32 y = startY + static_cast<int32>(row) * blockSize;

      // ブロックの色をランダムに選択（位置に基づいた擬似ランダム）
      // 位置を使ったハッシュ値でシード化し、ランダムに見えるようにする
      const size_t seed = (row * 982451653ULL + col * 1572869ULL) % blockColors.size();
      const ColorF blockColor = blockColors[seed];

      // 角丸の矩形を描画
      RoundRect{ x, y, blockSize, blockSize, 15 }.draw(blockColor);

      // ブロックの枠線を描画
      RoundRect{ x, y, blockSize, blockSize, 15 }.drawFrame(2, ColorF{ 0.2, 0.2, 0.2, 0.5 });

      // ブロック内のテキストを中央に描画（メンバー変数のフォントを使用）
      block_font_(blockText).drawAt(x + blockSize / 2, y + blockSize / 2, ColorF{ 1.0 });
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

//void Game::drawFadeIn(double t) const
//{
//  draw();
//
//  // 1280x720対応のフェードイン効果
//  for (int32 y = 0; y < 8; ++y) {
//    RectF{ (1280 + y * 120 - (1 + t) * 2560), (y * 90), 2560, 90 }.draw(HSV{ (y * 20), 0.2, 1.0 });
//  }
//}

void Game::drawFadeOut(double t) const
{
  draw();

  // 1280x720対応のフェードアウト効果
  Circle{ 640, 360, 640 }
  .drawFrame((t * 640), 0, ColorF{ 0.2, 0.3, 0.4 });
}
