#pragma once
#include <iostream>
#include <deque>
#include <algorithm>

#include "Scenes/Enum.h"
#include "System/SaveData/SaveData.hpp"
#include "System/Menu/Menu.h"
#include "InGame/Ui.h"
#include "InGame/BlockDestroyEffect.h"
#include "InGame/BubbleEffect.h"
#include "InGame/HitEffect.h"
#include "InGame/WordDisplayBox.h"
#include "InGame/CollectedCharacters.h"
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

  //void drawFadeIn(double t) const override;

  //void drawFadeOut(double t) const override;

private:

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
  /// ヒントを更新
  /// </summary>
  void UpdateHint();

  /// <summary>
  /// ゲームオーバー処理を開始
  /// </summary>
  void StartGameOver();

  /// <summary>
  /// ゲームクリア処理を開始
  /// </summary>
  void StartGameClear();

  /// <summary>
  /// 次のブロック塊を生成
  /// </summary>
  void GenerateNextBlockChunk();

  /// <summary>
  /// スクロール更新（新しいブロック塊の生成判定）
  /// </summary>
  void UpdateScroll();

  /// <summary>
  /// ブロックのテクスチャ
  /// </summary>
  Texture block_bg_texture_;

  Stopwatch m_stopwatch_;

  bool is_paused_ = false;  // ゲーム一時停止フラグ
  bool is_game_over_ = false; // ゲームオーバーフラグ
  bool is_game_clear_ = false; // ゲームクリアフラグ
  double game_over_timer_ = 0.0; // ゲームオーバー演出時間
  double game_clear_timer_ = 0.0; // ゲームクリア演出時間

  // メニュー
  std::unique_ptr<Menu> menu_;

  // UI管理
  std::shared_ptr<Ui> ui_;

  // プレイヤー
  std::shared_ptr<Player> player_;

  Array<Texture> textures_;

  Array<Texture> block_textures_;

  // ✨アイコン
  Texture sparkle_tex_;
  // 武器テクスチャ
  Texture weapon_texture_;

  // エア残量（デモ用）
  float air_amount_ = 1.0f;

  // ブロックマネージャー
  BlockManager block_manager_;

  // ブロック構造体
  struct Block
  {
    /// <summary>
    /// ブロックの種類
    /// </summary>
    enum class Type
    {
      kNormal,      // 通常の破壊可能ブロック
      kWall,        // 壊せない壁
      kEmpty        // 空
    };

    String value;           // ブロックに表示される文字列
    bool is_destroyed;      // ブロックが破壊されているか
    Vec2 position;          // ブロックのピクセル位置（左上）
    Type type;              // ブロックの種類
    bool is_first;          // 単語の先頭文字フラグ
    ColorF color = Palette::White; // ブロックの色
    
    Block() 
      : value(U"")
      , is_destroyed(false)
      , type(Type::kEmpty)
      , is_first(false)
      , color(Palette::White)
    {}
    
    Block(const String& val, bool first = false) 
      : value(val)
      , is_destroyed(false)
      , type(Type::kNormal)
      , is_first(first)
      , color(Palette::White)
    {}

    Block(Type blockType)
      : value(U"")
      , is_destroyed(false)
      , type(blockType)
      , is_first(false)
      , color(Palette::White)
    {}
    
    // ブロックが空かどうか
    bool isEmpty() const
    {
      return (value.isEmpty() || is_destroyed) && type != Type::kWall;
    }

    // ブロックが破壊可能かどうか
    bool isDestroyable() const
    {
      return type == Type::kNormal && !is_destroyed;
    }

    // 壁ブロックかどうか
    bool isWall() const
    {
      return type == Type::kWall;
    }
  };

  // ブロックグリッド
  Array<Array<Block>> block_grid_;

  // ブロック描画用フォント
  Font block_font_;

  // 完成単語リスト用フォント
  Font completed_word_font_;
  Font hint_font_;

  // デバッグ用フォント
  Font debug_font_;

  // ゲームオーバー用フォント
  Font game_over_font_;

  // プレイヤーの落下速度
  float player_fall_velocity_ = 0.0f;

  // プレイヤーの移動入力
  Vec2 player_move_input_ = Vec2::Zero();

  /// <summary>
  /// 獲得した文字を管理
  /// </summary>
  std::unique_ptr<CollectedCharacters> collected_characters_;

  /// <summary>
  /// 今まで完成した単語（廃止予定：collected_characters_を使用）
  /// </summary>
  Array<String> have_words_;

  // 最大文字数（廃止予定：CollectedCharactersで管理）
  size_t max_string_ = 7;

  // 完成した単語のリスト
  Array<String> completed_words_;
  double hint_timer_ = 0.0;
  String current_hint_;
  double message_timer_ = 0.0;
  String current_message_;

  // カメラオフセット（ワールド座標からスクリーン座標への変換）
  Vec2 camera_offset_ = Vec2::Zero();

  // ブロック破壊演出
  std::unique_ptr<BlockDestroyEffect> block_destroy_effect_;

  // 泡エフェクト（ゲームオーバー演出）
  std::unique_ptr<BubbleEffect> bubble_effect_;

  // ヒット演出（取得単語表示）
  HitEffect hit_effect_;

  // 文字表示ボックス（もじぴったん風UI）
  std::unique_ptr<WordDisplayBox> word_display_box_;

  // 現在のブロック塊のY座標（下端）
  float current_chunk_bottom_y_ = 0.0f;
  
  // 次のブロック塊を生成するトリガー位置
  float next_chunk_trigger_y_ = 0.0f;
  
  // ブロック塊間の距離
  static constexpr float kChunkSpacing = 500.0f;

  // デバッグモード
#if _DEBUG
  static constexpr bool kDebugMode = true;
#else
  static constexpr bool kDebugMode = false;
#endif
};
