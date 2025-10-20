#pragma once
#include "Scenes/Enum.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/Menu.h"
#include "InGame/Ui.h"
#include "Player.hpp"

// ゲームシーン
class Game : public SceneManager<EnumScene, SaveData>::Scene
{
public:

  Game(const InitData& init);

  ~Game();

  void update() override;

  void draw() const override;

  void drawFadeIn(double t) const override;

  void drawFadeOut(double t) const override;

private:

  // 代表的な絵文字ブロックから単一コードポイント絵文字を収集
  static Array<String> BuildAllEmojis()
  {
    Array<String> e;

    auto addRange = [&](uint32 start, uint32 end)
      {
        for (uint32 cp = start; cp <= end; ++cp) {
          String s;
          s.push_back(static_cast<char32_t>(cp));
          e << s;
        }
      };

    // 主な絵文字ブロック
    addRange(0x1F600, 0x1F64F);  // Emoticons

    return e;
  }

  Texture m_emoji;

  Stopwatch m_stopwatch_;

  bool is_begin_ = false;
  bool is_paused_ = false;  // ゲーム一時停止フラグ

  // メニュー
  std::unique_ptr<Menu> menu_;

  // UI管理
  std::shared_ptr<Ui> ui_;

  // プレイヤー
  std::shared_ptr<Player> player_;

  // 登場する絵文字
  const Array<String> emojis = BuildAllEmojis();

  Array<MultiPolygon> polygons;

  Array<Texture> textures;

  // 2D 物理演算のシミュレーションステップ（秒）
  // constexpr を外して const に変更
  const double StepTime = (1.0 / 200.0);

  // 2D 物理演算のシミュレーション蓄積時間（秒）
  double accumulatedTime = 0.0;

  // 2D 物理演算のワールド
  P2World world;

  // [_] 地面
  const P2Body ground = world.createLine(P2Static, Vec2{ 0, 0 }, Line{ -300, 0, 300, 0 });

  // 動物の物体
  Array<P2Body> bodies;

  // 物体の ID と絵文字のインデックスの対応テーブル
  HashTable<P2BodyID, size_t> table;

  // 絵文字のインデックス
  size_t index = 1;
  //size_t index = Random(polygons.size() - 1);

  // 2D カメラ
  Camera2D camera{ Vec2{ 0, -200 } };

  // エア残量（デモ用）
  float air_amount_ = 1.0f;
};

