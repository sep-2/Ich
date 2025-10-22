#include "stdafx.h"
#include "Player.hpp"
#include "System/Renderer/TextureWrapper.h"
#include "System/Renderer/Renderer.h"
#include "System/Renderer/Priority.h"

namespace PlayerConstants {
  const String kPlayerWalkImagePath = U"Assets/Image/Player/player_walk.png";
}

/// <summary>
/// コンストラクタ
/// </summary>
Player::Player()
  : Task()
  , position_(100.0f, 200.0f)  // 画面中央に配置
  , move_speed_(200.0f)        // 200ピクセル/秒
  , current_frame_(0)
  , animation_timer_(0.0f)
  , frame_duration_(8.0f / 60.0f)  // 8フレーム = 8/60秒 ≈ 0.133秒
  , facing_left_(false)
  , is_moving_(false)
{
  // プレイヤーのスプライトテクスチャを読み込み
  player_texture_ = std::make_shared<Texture>(PlayerConstants::kPlayerWalkImagePath);

  // TextureWrapperを初期化（初期フレームで作成）
  Rect initial_frame = GetCurrentSpriteFrame();
  player_wrapper_ = std::make_shared<TextureWrapper>(player_texture_,
    static_cast<int>(position_.x), static_cast<int>(position_.y), initial_frame);

  // 中央揃えで表示
  player_wrapper_->SetIsCenter(true);
}

/// <summary>
/// デストラクタ
/// </summary>
Player::~Player()
{
}

/// <summary>
/// 毎フレーム更新処理
/// </summary>
/// <param name="delta_time">前回実行フレームからの経過時間（秒）</param>
void Player::Update(float delta_time)
{
  // 移動処理
  if (is_moving_) {
    // アニメーション更新
    UpdateAnimation(delta_time);
  } else {
    // 停止時は最初のフレームに戻す
    current_frame_ = 0;
    animation_timer_ = 0.0f;
  }

  // TextureWrapperの位置とUV座標を更新
  if (player_wrapper_) {
    player_wrapper_->SetPosition(static_cast<int>(position_.x), static_cast<int>(position_.y));
    player_wrapper_->SetUVRect(GetCurrentSpriteFrame());

    // 左右反転（左を向く場合は-1.0でスケール）
    if (facing_left_) {
      player_wrapper_->SetScale(-0.5f, 0.5);
    } else {
      player_wrapper_->SetScale(0.5f, 0.5f);
    }
  }
}

/// <summary>
/// 毎フレームの描画処理
/// </summary>
void Player::Render()
{
  Renderer* renderer = Renderer::GetInstance();

  if (renderer && player_wrapper_) {
    renderer->Push(Priority::kPlayerPriority, std::static_pointer_cast<Task>(player_wrapper_));
  }
}

/// <summary>
/// プレイヤーの位置を取得
/// </summary>
/// <returns>プレイヤーの位置</returns>
Vec2 Player::GetPosition() const
{
  return position_;
}

/// <summary>
/// プレイヤーの位置を設定
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
void Player::SetPosition(float x, float y)
{
  position_.x = x;
  position_.y = y;
}

/// <summary>
/// プレイヤーの移動速度を設定
/// </summary>
/// <param name="speed">移動速度（ピクセル/秒）</param>
void Player::SetMoveSpeed(float speed)
{
  move_speed_ = speed;
}

/// <summary>
/// 移動状態を設定
/// </summary>
/// <param name="isMoving">移動中かどうか</param>
void Player::SetMoving(bool isMoving)
{
  is_moving_ = isMoving;
}

/// <summary>
/// 向きを設定
/// </summary>
/// <param name="facingLeft">左を向いているか</param>
void Player::SetFacingLeft(bool facingLeft)
{
  facing_left_ = facingLeft;
}

/// <summary>
/// プレイヤーの実際の幅を取得（スケール適用後）
/// </summary>
/// <returns>プレイヤーの幅</returns>
float Player::GetWidth() const
{
  return kSpriteWidth * kScale;
}

/// <summary>
/// プレイヤーの実際の高さを取得（スケール適用後）
/// </summary>
/// <returns>プレイヤーの高さ</returns>
float Player::GetHeight() const
{
  return kSpriteHeight * kScale;
}

/// <summary>
/// 入力処理
/// </summary>
void Player::HandleInput()
{
  // 入力処理は InGame クラスで行うため、ここでは何もしない
  is_moving_ = false;
}

/// <summary>
/// アニメーション更新
/// </summary>
/// <param name="delta_time">デルタタイム</param>
void Player::UpdateAnimation(float delta_time)
{
  animation_timer_ += delta_time;

  // 指定フレーム数（8フレーム）経過したら次のアニメーションフレームに進む
  if (animation_timer_ >= frame_duration_) {
    //current_frame_ = (current_frame_ + 1) % kAnimationFrames;
    animation_timer_ = 0.0f;
  }
}

/// <summary>
/// 現在のスプライトフレームを取得
/// </summary>
/// <returns>スプライトのUV座標</returns>
Rect Player::GetCurrentSpriteFrame() const
{
  return Rect(current_frame_ * kSpriteWidth, 0, kSpriteWidth, kSpriteHeight);
}
