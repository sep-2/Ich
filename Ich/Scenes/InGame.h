#pragma once
#include <iostream>
#include <deque>
#include <algorithm>

#include "Scenes/Enum.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/Menu.h"
#include "InGame/Ui.h"
#include "Player.hpp"
#include "System/System/BlockManager.h"

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

  /// <summary>
  /// プレイヤーの足元のブロックを破壊する
  /// </summary>
  void DestroyBlockUnderPlayer();

  /// <summary>
  /// プレイヤーのグリッド位置を取得
  /// </summary>
  /// <param name="gridRow">グリッド行</param>
  /// <param name="gridCol">グリッド列</param>
  /// <returns>有効な位置の場合true</returns>
  bool GetPlayerGridPosition(int32& gridRow, int32& gridCol) const;

  /// <summary>
  /// ピクセル座標をグリッド座標に変換
  /// </summary>
  /// <param name="pixelPos">ピクセル座標</param>
  /// <param name="gridRow">グリッド行（出力）</param>
  /// <param name="gridCol">グリッド列（出力）</param>
  /// <returns>有効な範囲内の場合true</returns>
  bool PixelToGrid(const Vec2& pixelPos, int32& gridRow, int32& gridCol) const;

  /// <summary>
  /// グリッド座標をピクセル座標（中心）に変換
  /// </summary>
  /// <param name="gridRow">グリッド行</param>
  /// <param name="gridCol">グリッド列</param>
  /// <returns>グリッドの中心のピクセル座標</returns>
  Vec2 GridToPixel(int32 gridRow, int32 gridCol) const;

  /// <summary>
  /// グリッドの境界ピクセル座標を取得
  /// </summary>
  /// <param name="gridRow">グリッド行</param>
  /// <param name="gridCol">グリッド列</param>
  /// <returns>グリッドの左上座標</returns>
  Vec2 GetGridTopLeft(int32 gridRow, int32 gridCol) const;

  /// <summary>
  /// プレイヤーを落下させる
  /// </summary>
  void UpdatePlayerFall(float delta_time);

  /// <summary>
  /// 指定位置にブロックが存在するかチェック
  /// </summary>
  /// <param name="gridRow">グリッド行</param>
  /// <param name="gridCol">グリッド列</param>
  /// <returns>ブロックが存在する場合true</returns>
  bool HasBlockAt(int32 gridRow, int32 gridCol) const;

  /// <summary>
  /// プレイヤーの移動を処理（衝突判定付き）
  /// </summary>
  void UpdatePlayerMovement(float delta_time);

  /// <summary>
  /// カメラ位置を更新（プレイヤーの位置に追従）
  /// </summary>
  void UpdateCamera();

  /// <summary>
  /// デバッグ情報を描画
  /// </summary>
  void DrawDebugInfo() const;

  /// <summary>
  /// �w�i�摜
  /// </summary>
  Texture block_bg_texture_;

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

  Array<Texture> block_textures_;

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

  // ブロックマネージャー
  BlockManager block_manager_;

  // ブロック構造体
  struct Block
  {
    String value;           // ブロックに表示される文字列
    bool is_destroyed;      // ブロックが破壊されているか
    Vec2 position;      // ブロックのピクセル位置（左上）
    
    Block() 
      : value(U"")
      , is_destroyed(false)
    {}
    
    Block(const String& val) 
      : value(val)
      , is_destroyed(false)
    {}
    
    // ブロックが空かどうか
    bool isEmpty() const
    {
      return value.isEmpty() || is_destroyed;
    }
  };

  // ブロックグリッド
  Array<Array<Block>> block_grid_;

  // ブロック描画用フォント
  Font block_font_;

  // デバッグ用フォント
  Font debug_font_;

  // ブロック描画の定数
  static constexpr int32 kBlockSize = 100;
  static constexpr int32 kStartX = 100;
  static constexpr int32 kStartY = 250;

  // プレイヤーの落下速度
  float player_fall_velocity_ = 0.0f;
  static constexpr float kGravity = 800.0f;  // ピクセル/秒^2
  static constexpr float kMaxFallSpeed = 600.0f;  // 最大落下速度

  // プレイヤーの移動入力
  Vec2 player_move_input_ = Vec2::Zero();

  Array<String> have_words_;

  // 最大文字数（デフォルト5）
  size_t max_string_ = 5;

  // 完成した単語のリスト
  Array<String> completed_words_;

  // 完成した単語の最大保存数（デフォルト10）
  size_t max_completed_words_ = 10;

  // カメラオフセット（ワールド座標からスクリーン座標への変換）
  Vec2 camera_offset_ = Vec2::Zero();

  // カメラが追従する際のスムーズさ（0.0～1.0、1.0で即座に追従）
  static constexpr float kCameraFollowSpeed = 0.1f;

  // デバッグモード
#if _DEBUG
  static constexpr bool kDebugMode = true;
#else
  static constexpr bool kDebugMode = false;
#endif
};

