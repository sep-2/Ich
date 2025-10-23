#pragma once
#include "System/Task/Task.h"
#include <memory>
#include <Siv3D.hpp>

class TextureWrapper;

/// <summary>
/// プレイヤークラス - 歩行アニメーション付きキャラクター
/// </summary>
class Player : public Task
{
public:
  enum class State
  {
    kIdle,
    kWalking,
    kFalling,
  };

  /// <summary>
  /// コンストラクタ
  /// </summary>
  Player();

  /// <summary>
  /// デストラクタ
  /// </summary>
  virtual ~Player();

  /// <summary>
  /// 毎フレーム更新処理
  /// </summary>
  /// <param name="delta_time">前回実行フレームからの経過時間（秒）</param>
  void Update(float delta_time) override;

  /// <summary>
  /// 毎フレームの描画処理
  /// </summary>
  void Render() override;

  /// <summary>
  /// 移動処理
  /// </summary>
  /// <param name="delta_time"></param>
  void Move(float delta_time);

  /// <summary>
  /// プレイヤーの位置を取得
  /// </summary>
  /// <returns>プレイヤーの位置</returns>
  Vec2 GetPosition() const;

  /// <summary>
  /// プレイヤーの位置を設定
  /// </summary>
  /// <param name="x">X座標</param>
  /// <param name="y">Y座標</param>
  void SetPosition(float x, float y);

  /// <summary>
  /// プレイヤーの移動速度を設定
  /// </summary>
  /// <param name="speed">移動速度（ピクセル/秒）</param>
  void SetMoveSpeed(float speed);

  /// <summary>
  /// 移動状態を設定
  /// </summary>
  /// <param name="isMoving">移動中かどうか</param>
  void SetMoving(bool isMoving);

  /// <summary>
  /// 向きを設定
  /// </summary>
  /// <param name="facingLeft">左を向いているか</param>
  void SetFacingLeft(bool facingLeft);

  /// <summary>
  /// プレイヤーの実際の幅を取得（スケール適用後）
  /// </summary>
  /// <returns>プレイヤーの幅</returns>
  float GetWidth() const;

  /// <summary>
  /// プレイヤーの実際の高さを取得（スケール適用後）
  /// </summary>
  /// <returns>プレイヤーの高さ</returns>
  float GetHeight() const;

  Vec2 GetLeftTop() const;
  Vec2 GetRightBottom() const;
  Vec2 GetRightTop() const;
  Vec2 GetLeftBottom() const;

  State state_ = State::kFalling;

  /// <summary>
  /// プレイヤーの移動速度（ピクセル/秒）
  /// </summary>
  float move_speed_;

private:
  /// <summary>
  /// 入力処理（現在は使用しない）
  /// </summary>
  void HandleInput();

  /// <summary>
  /// アニメーション更新
  /// </summary>
  /// <param name="delta_time">デルタタイム</param>
  void UpdateAnimation(float delta_time);

  /// <summary>
  /// 現在のスプライトフレームを取得
  /// </summary>
  /// <returns>スプライトのUV座標</returns>
  Rect GetCurrentSpriteFrame() const;

  /// <summary>
  /// プレイヤーの歩行スプライトテクスチャ
  /// </summary>
  std::shared_ptr<Texture> player_texture_;

  /// <summary>
  /// プレイヤー表示用ラッパー
  /// </summary>
  std::shared_ptr<TextureWrapper> player_wrapper_;

  /// <summary>
  /// プレイヤーの位置
  /// </summary>
  Vec2 position_;

  /// <summary>
  /// 現在のアニメーションフレーム（0-4）
  /// </summary>
  int current_frame_;

  /// <summary>
  /// アニメーションタイマー
  /// </summary>
  float animation_timer_;

  /// <summary>
  /// 1フレーム当たりの時間（秒）
  /// </summary>
  float frame_duration_;

  /// <summary>
  /// プレイヤーが左を向いているか
  /// </summary>
  bool facing_left_;

  /// <summary>
  /// プレイヤーが移動中か
  /// </summary>
  bool is_moving_;

  /// <summary>
  /// スプライトの1フレームのサイズ
  /// </summary>
  static const int kSpriteWidth = 128;
  static const int kSpriteHeight = 408;

  /// <summary>
  /// アニメーションフレーム数
  /// </summary>
  static const int kAnimationFrames = 5;

  /// <summary>
  /// スケール（0.5倍に縮小）
  /// </summary>
  static constexpr float kScale = 0.5f;
};

