#include "stdafx.h"

#include "InGame.h"

#include "System/Audio/AudioManager.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/GameSettings.h"


Game::Game(const InitData& init)
  : IScene{ init }
  , m_emoji{ U"🐥"_emoji }
  , menu_(std::make_unique<Menu>())
  , ui_(std::make_shared<Ui>())
  , air_amount_(1.0f)
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

  // UIの初期設定
  ui_->SetAirGaugePosition(50, 50);  // 画面左上にエアゲージを配置
  ui_->SetAirGauge(air_amount_);
}

Game::~Game()
{
  //PRINT << U"Game::~Game()";
}

void Game::update()
{
  // Esc キーでメニュー開閉
  if (KeyEscape.down() || KeyZ.down()) {
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

  // 以下、通常のゲームロジック
  accumulatedTime += Scene::DeltaTime();

  while (StepTime <= accumulatedTime) {
    // 2D 物理演算のワールドを更新する
    world.update(StepTime);

    accumulatedTime -= StepTime;
  }

  // 地面より下に落ちた物体は削除する
  for (auto it = bodies.begin(); it != bodies.end();) {
    if (100 < it->getPos().y) {
      // 対応テーブルからも削除
      table.erase(it->id());

      it = bodies.erase(it);

      AudioManager::GetInstance()->PlaySe(SeKind::kDecideSe);
      changeScene(EnumScene::kTitle);
    } else {
      ++it;
    }
  }

  // 2D カメラを更新する
  camera.update();
  {
    // 2D カメラから Transformer2D を作成する
    const auto t = camera.createTransformer();

    // 左クリックされたら
    if (MouseL.down()) {
      // ボディを追加する
      bodies << world.createPolygons(P2Dynamic, Cursor::PosF(), polygons[index], P2Material{ 0.1, 0.0, 1.0 });

      // ボディ ID と絵文字のインデックスの組を対応テーブルに追加する
      table.emplace(bodies.back().id(), std::exchange(index, Random(polygons.size() - 1)));

      auto& data = getData<SaveData>();
      data.click_count_++;

      // メインループの後、終了時にゲームをセーブ
      {
        // バイナリファイルをオープン
        Serializer<BinaryWriter> writer{ U"game.save" };

        // シリアライズに対応したデータを書き出す
        writer(data);
      }
    }

    // すべてのボディを描画する
    for (const auto& body : bodies) {
      textures[table[body.id()]].rotated(body.getAngle()).drawAt(body.getPos());
    }

    // 地面を描画する
    ground.draw(Palette::Green);

    // 現在操作できる絵文字を描画する
    textures[index].drawAt(Cursor::PosF(), ColorF{ 1.0, (0.5 + Periodic::Sine0_1(1s) * 0.5) });
  }

  //// 2D カメラの操作を描画する
  camera.draw(Palette::Orange);
}

void Game::draw() const
{
  Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

  // UIの描画（メニューより先に描画）
  if (ui_) {
    ui_->Render();
  }

  // メニューが開いている場合は描画
  if (menu_->IsOpen()) {
    menu_->Draw();
  }

  // 明るさ設定を適用
  GameSettings::GetInstance()->ApplyBrightness();
}

void Game::drawFadeIn(double t) const
{
  draw();

  for (int32 y = 0; y < 6; ++y) {
    RectF{ (800 + y * 100 - (1 + t) * 1600), (y * 100), 1600, 100 }.draw(HSV{ (y * 20), 0.2, 1.0 });
  }
}

void Game::drawFadeOut(double t) const
{
  draw();

  Circle{ 400, 300, 600 }
  .drawFrame((t * 600), 0, ColorF{ 0.2, 0.3, 0.4 });
}
