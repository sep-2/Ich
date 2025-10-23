#include "stdafx.h"
#include "Player.hpp"
#include "System/Renderer/TextureWrapper.h"
#include "System/Renderer/Renderer.h"
#include "System/Renderer/Priority.h"
#include <cmath>

namespace PlayerConstants {
  struct PoseTextureEntry
  {
    Player::Pose pose;
    FilePath path;
  };

  // ポーズ毎に使用するテクスチャの対応表。
  // 右向き専用素材が無い場合は「左右反転で流用する」想定で Left1.png を割り当てている。
  // （反転処理は後段の UpdateTextureForPose 内で行う）
  const Array<PoseTextureEntry> kPoseTextures = {
    { Player::Pose::kIdle,             U"Assets/Image/Player/Idle1.png" },
    { Player::Pose::kStrafeLeft,       U"Assets/Image/Player/Left1.png" },
    { Player::Pose::kStrafeRight,      U"Assets/Image/Player/Left1.png" }, // 右向きは水平反転で対応
    { Player::Pose::kWalkForwardLeft,  U"Assets/Image/Player/WalkA1.png" },
    { Player::Pose::kWalkForwardRight, U"Assets/Image/Player/WalkB1.png" },
    { Player::Pose::kFall,             U"Assets/Image/Player/Fall1.png" },
    { Player::Pose::kGameOver,         U"Assets/Image/Player/GameOver1.png" },
  };
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
  , pose_(Pose::kIdle)
{
  // ★フェーズ3要件★
  // ここからは歩行/待機などポーズ単位で正式版画像を差し替える準備を行う。
  // 1) 起動時に必要なPNGを読み込んでキャッシュする
  // 2) 初期ポーズに応じたテクスチャをTextureWrapperへ設定する
  LoadPoseTextures();

  const auto initialTexture = GetTextureForPose(pose_);
  if (initialTexture)
  {
    player_wrapper_ = std::make_shared<TextureWrapper>(initialTexture,
      static_cast<int>(position_.x), static_cast<int>(position_.y));
    player_wrapper_->SetIsCenter(true);
  }

  if (player_wrapper_)
  {
    UpdateTextureForPose(); // 初期状態でもポーズに応じたスケール・反転を適用しておく
  }
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

  if (player_wrapper_)
  {
    player_wrapper_->SetPosition(static_cast<int>(position_.x), static_cast<int>(position_.y));
  }
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
  ApplyPoseFromMovement(false); // 移動フラグを更新したらポーズも追従させる
}

/// <summary>
/// 向きを設定
/// </summary>
/// <param name="facingLeft">左を向いているか</param>
void Player::SetFacingLeft(bool facingLeft)
{
  facing_left_ = facingLeft;
  ApplyPoseFromMovement(false); // 右左の向きが変わった瞬間に画像も反転させたい
}

/// <summary>
/// プレイヤーの実際の幅を取得（スケール適用後）
/// </summary>
/// <returns>プレイヤーの幅</returns>
float Player::GetWidth() const
{
  if (player_wrapper_)
  {
    if (const auto texture = player_wrapper_->GetTexture())
    {
      return static_cast<float>(texture->width()) * std::abs(player_wrapper_->GetScaleX()); // 実際の描画倍率を反映
    }
  }

  const float fallbackScale = kTargetHeight / static_cast<float>(kSpriteHeight);
  return kSpriteWidth * fallbackScale;
}

/// <summary>
/// プレイヤーの実際の高さを取得（スケール適用後）
/// </summary>
/// <returns>プレイヤーの高さ</returns>
float Player::GetHeight() const
{
  if (player_wrapper_)
  {
    if (const auto texture = player_wrapper_->GetTexture())
    {
      return static_cast<float>(texture->height()) * std::abs(player_wrapper_->GetScaleY()); // 縦方向も実スケールで返却
    }
  }

  return kTargetHeight;
}

/// <summary>
/// 現在のポーズを取得
/// </summary>
Player::Pose Player::GetPose() const
{
  return pose_;
}

/// <summary>
/// 現在のポーズを設定
/// </summary>
/// <param name="pose">設定するポーズ</param>
void Player::SetPose(const Pose pose)
{
  if (pose_ == pose)
  {
    return;
  }

  pose_ = pose;
  UpdateTextureForPose();
}

/// <summary>
/// 移動フラグと向きからポーズを再計算
/// </summary>
void Player::RefreshPoseFromMovement()
{
  ApplyPoseFromMovement(true); // 落下終了など強制的に移動ポーズへ戻したい時はこちらを使用
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
/// 移動・向き情報から妥当なポーズを算出
/// </summary>
Player::Pose Player::CalculateMovementPose() const
{
  if (is_moving_) {
    return facing_left_ ? Pose::kStrafeLeft : Pose::kStrafeRight;
  }

  return Pose::kIdle;
}

void Player::ApplyPoseFromMovement(const bool force)
{
  if (!force && (pose_ == Pose::kFall || pose_ == Pose::kGameOver)) {
    // 落下/ゲームオーバー中は移動フラグがどうであれ姿勢を固定したいので更新しない
    return;
  }

  const Pose newPose = CalculateMovementPose();
  if (force || pose_ != newPose)
  {
    pose_ = newPose;
    UpdateTextureForPose(); // 画像・スケール・反転を即時更新
  }
}

void Player::LoadPoseTextures()
{
  // 起動時にまとめてPNGを読み込む。HashTable に保持しておけばポーズ切替時に都度読み直さなくて済む。
  for (const auto& entry : PlayerConstants::kPoseTextures)
  {
    pose_textures_.emplace(entry.pose, std::make_shared<Texture>(entry.path));
  }
}

std::shared_ptr<Texture> Player::GetTextureForPose(const Pose pose) const
{
  // 存在しないポーズが指定される可能性もあるので find で安全に探索する
  if (const auto it = pose_textures_.find(pose); it != pose_textures_.end())
  {
    return it->second;
  }

  return nullptr;
}

void Player::UpdateTextureForPose()
{
  const auto texture = GetTextureForPose(pose_);
  if (!texture)
  {
    // 想定外だが読み込み失敗などでテクスチャが取れなかった場合は何もせず早期リターン
    return;
  }

  if (!player_wrapper_)
  {
    player_wrapper_ = std::make_shared<TextureWrapper>(texture,
      static_cast<int>(position_.x), static_cast<int>(position_.y));
    player_wrapper_->SetIsCenter(true);
  }
  else
  {
    player_wrapper_->SetTexture(texture);
  }

  player_wrapper_->SetPosition(static_cast<int>(position_.x), static_cast<int>(position_.y));

  const float textureHeight = static_cast<float>(texture->height());
  // 目標とする見た目の高さ(kTargetHeight)から縮尺を逆算。
  // 画像サイズが想定外でも最終的には「おおむね既定の大きさ」で描画できる。
  float scaleY = (textureHeight > 0.0f) ? (kTargetHeight / textureHeight) : kScale;
  if (!std::isfinite(scaleY) || scaleY <= 0.0f)
  {
    scaleY = kScale;
  }

  float scaleX = scaleY;

  switch (pose_)
  {
  case Pose::kStrafeRight:
    scaleX = -scaleY;
    break;
  case Pose::kWalkForwardRight:
    scaleX = -scaleY;
    break;
  default:
    scaleX = scaleY;
    break;
  }

  // TextureWrapper へ反映。左右反転した場合もCollider計算のため絶対値を保持している。
  player_wrapper_->SetScale(scaleX, scaleY);
}

