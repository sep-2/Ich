#include "stdafx.h"
#include "Player.hpp"
#include "System/Renderer/TextureWrapper.h"
#include "System/Renderer/Renderer.h"
#include "System/Renderer/Priority.h"
#include <cmath>

class WeaponRenderTask : public Task
{
public:
  WeaponRenderTask()
    : visible_(false)
    , center_(Vec2{ 0.0, 0.0 })
    , size_(SizeF{ 0.0, 0.0 })
    , rotation_(0.0)
    , color_(Palette::White)
  {
  }

  void SetState(const Vec2& center, const SizeF& size, double rotation, const ColorF& color, bool visible)
  {
    center_ = center;
    size_ = size;
    rotation_ = rotation;
    color_ = color;
    visible_ = visible;
  }

  void Render() override
  {
    if (!visible_)
    {
      return;
    }

    RectF{ Arg::center(center_), size_ }.rotated(rotation_).draw(color_);
  }

private:
  bool visible_;
  Vec2 center_;
  SizeF size_;
  double rotation_;
  ColorF color_;
};

namespace PlayerConstants {
  struct PoseAnimationResource
  {
    Player::Pose pose;
    Array<FilePath> frames;
  };

  // ポーズ毎に使用するテクスチャの対応表。
  // 右向き専用素材が無いものは、左向き素材を読み込みつつ UpdateTextureForPose で左右反転する。
  // 各配列の順番はアニメーションの周期（0.2秒単位）で順番に再生される。
  const Array<PoseAnimationResource> kPoseAnimationResources = {
    { Player::Pose::kIdle,             { U"Assets/Image/Player/Idle1.png",    U"Assets/Image/Player/Idle2.png" } },
    { Player::Pose::kStrafeLeft,       { U"Assets/Image/Player/Left1.png",    U"Assets/Image/Player/Left2.png" } },
    { Player::Pose::kStrafeRight,      { U"Assets/Image/Player/Left1.png",    U"Assets/Image/Player/Left2.png" } }, // 右は左右反転
    { Player::Pose::kWalkForwardLeft,  { U"Assets/Image/Player/WalkA1.png",   U"Assets/Image/Player/WalkA2.png" } },
    { Player::Pose::kWalkForwardRight, { U"Assets/Image/Player/WalkA1.png",   U"Assets/Image/Player/WalkA2.png" } }, // 右は左右反転
    { Player::Pose::kFall,             { U"Assets/Image/Player/Fall1.png",    U"Assets/Image/Player/Fall2.png" } },
    { Player::Pose::kGameOver,         { U"Assets/Image/Player/GameOver1.png",U"Assets/Image/Player/GameOver2.png" } },
  };
}

/// <summary>
/// コンストラクタ
/// </summary>
Player::Player()
  : Task()
  , position_(100.0f, 200.0f)  // 画面中央に配置
  , move_speed_(200.0f)        // 200ピクセル/秒
  , current_pose_frame_(0)
  , animation_timer_(0.0f)
  , frame_interval_seconds_(0.2f)  // 正式版素材は0.2秒感覚で切り替える想定
  , facing_left_(false)
  , is_moving_(false)
  , pose_(Pose::kIdle)
  , weapon_forward_dir_(Vec2{ 1.0, 0.0 })
  , weapon_base_position_(position_)
  , weapon_position_(position_)
  , weapon_angle_(0.0)
  , weapon_render_rotation_(kWeaponBaseRotation)
  , weapon_active_(false)
  , weapon_render_task_(std::make_shared<WeaponRenderTask>())
  , weapon_color_(ColorF{ 0.9, 0.25, 0.25, 0.85 })
{
  // ★フェーズ3要件★
  // ここからは歩行/待機などポーズ単位で正式版画像を差し替える準備を行う。
  // 1) 起動時に必要なPNGを読み込んでキャッシュする
  // 2) 初期ポーズに応じたテクスチャをTextureWrapperへ設定する
  LoadPoseTextures();

  const auto* initialFrames = FindPoseFrames(pose_);
  if (initialFrames && !initialFrames->isEmpty())
  {
    player_wrapper_ = std::make_shared<TextureWrapper>(initialFrames->front(),
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
  UpdateWeapon(delta_time);
  UpdateAnimation(delta_time);

  // TextureWrapperの位置とUV座標を更新
  if (player_wrapper_) {
    player_wrapper_->SetPosition(static_cast<int>(position_.x), static_cast<int>(position_.y));
    //player_wrapper_->SetUVRect(GetCurrentSpriteFrame());
  }
}

/// <summary>
/// 毎フレームの描画処理
/// </summary>
void Player::Render()
{
  Renderer* renderer = Renderer::GetInstance();

  if (!renderer)
  {
    return;
  }

  if (player_wrapper_) {
    renderer->Push(Priority::kPlayerPriority, std::static_pointer_cast<Task>(player_wrapper_));
  }

  if (weapon_render_task_ && weapon_active_)
  {
    weapon_render_task_->SetState(
      weapon_position_,
      SizeF{ kWeaponLength, kWeaponWidth },
      weapon_render_rotation_,
      weapon_color_,
      true);

    renderer->Push(Priority::kPlayerBomPriority, weapon_render_task_);
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
  weapon_forward_dir_ = facing_left_ ? Vec2{ -1.0, 0.0 } : Vec2{ 1.0, 0.0 };
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
  //return kSpriteHeight * kScale / 2;

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

  current_pose_frame_ = 0;
  animation_timer_ = 0.0f;
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
  //  入力処理は InGame クラスで行うため、ここでは何もしない
  is_moving_ = false;
}

void Player::UpdateWeapon(float delta_time)
{
  if (!weapon_render_task_)
  {
    return;
  }

  Vec2 direction{ 0.0, 1.0 };

  switch (pose_)
  {
  case Pose::kStrafeLeft:
    direction = Vec2{ -1.0, 0.0 };
    break;
  case Pose::kStrafeRight:
    direction = Vec2{ 1.0, 0.0 };
    break;
  case Pose::kWalkForwardLeft:
    direction = Vec2{ -1.0, 1.0 };
    break;
  case Pose::kWalkForwardRight:
    direction = Vec2{ 1.0, 1.0 };
    break;
  case Pose::kFall:
  case Pose::kGameOver:
  default:
    direction = Vec2{ 0.0, 1.0 };
    break;
  }

  const double directionLength = direction.length();
  if (directionLength <= 0.0)
  {
    weapon_forward_dir_ = Vec2{ 0.0, 1.0 };
  }
  else
  {
    weapon_forward_dir_ = direction / directionLength;
  }

  weapon_base_position_ = position_ + weapon_forward_dir_ * kWeaponForwardOffset;
  weapon_render_rotation_ = std::atan2(weapon_forward_dir_.y, weapon_forward_dir_.x);

  if (KeyZ.pressed())
  {
    weapon_active_ = true;
    weapon_angle_ += kWeaponAngularSpeed * delta_time;
    weapon_angle_ = std::fmod(weapon_angle_, Math::TwoPi);
    if (weapon_angle_ < 0.0)
    {
      weapon_angle_ += Math::TwoPi;
    }

    const Vec2 offset{
      std::cos(weapon_angle_) * kWeaponOrbitRadius,
      std::sin(weapon_angle_) * kWeaponOrbitRadius
    };
    weapon_position_ = weapon_base_position_ + offset;
  }
  else
  {
    weapon_active_ = false;
    weapon_position_ = weapon_base_position_;
  }
}

/// <summary>
/// アニメーション更新
/// </summary>
/// <param name="delta_time">デルタタイム</param>
void Player::UpdateAnimation(float delta_time)
{
  const auto* frames = FindPoseFrames(pose_);
  if (!frames || frames->isEmpty())
  {
    animation_timer_ = 0.0f;
    current_pose_frame_ = 0;
    return;
  }

  const size_t frameCount = frames->size();
  if (frameCount <= 1)
  {
    // 単一フレームの場合はループさせず、初期フレームのまま維持する
    animation_timer_ = 0.0f;
    current_pose_frame_ = 0;
    return;
  }

  animation_timer_ += delta_time;
  while (animation_timer_ >= frame_interval_seconds_)
  {
    animation_timer_ -= frame_interval_seconds_;
    current_pose_frame_ = (current_pose_frame_ + 1) % static_cast<int>(frameCount);
    UpdateTextureForPose();
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

Vec2 Player::GetLeftTop() const {
  return Vec2(position_.x - GetWidth() / 2.0f, position_.y - GetHeight() / 2.0f);
}
Vec2 Player::GetRightBottom() const {
  return Vec2(position_.x + GetWidth() / 2.0f, position_.y + GetHeight() / 2.0f);
}
Vec2 Player::GetRightTop() const {
  return Vec2(position_.x + GetWidth() / 2.0f, position_.y - GetHeight() / 2.0f);
}
Vec2 Player::GetLeftBottom() const {
  return Vec2(position_.x - GetWidth() / 2.0f, position_.y + GetHeight() / 2.0f);
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
    current_pose_frame_ = 0;
    animation_timer_ = 0.0f;
    pose_ = newPose;
    UpdateTextureForPose(); // 画像・スケール・反転を即時更新
  }
}

void Player::LoadPoseTextures()
{
  // 起動時にまとめてPNGを読み込む。HashTable に保持しておけばポーズ切替時に都度読み直さなくて済む。
  pose_textures_.clear();

  for (const auto& entry : PlayerConstants::kPoseAnimationResources)
  {
    Array<std::shared_ptr<Texture>> frames;
    frames.reserve(entry.frames.size());

    for (const auto& path : entry.frames)
    {
      frames << std::make_shared<Texture>(path);
    }

    pose_textures_.emplace(entry.pose, std::move(frames));
  }
}

const Array<std::shared_ptr<Texture>>* Player::FindPoseFrames(const Pose pose) const
{
  // 存在しないポーズが指定される可能性もあるので find で安全に探索する
  if (const auto it = pose_textures_.find(pose); it != pose_textures_.end())
  {
    return &(it->second);
  }

  return nullptr;
}

void Player::UpdateTextureForPose()
{
  const auto* frames = FindPoseFrames(pose_);
  if (!frames || frames->isEmpty())
  {
    // 想定外だが読み込み失敗などでテクスチャが取れなかった場合は何もせず早期リターン
    return;
  }

  const size_t frameCount = frames->size();
  size_t frameIndex = 0;
  if (frameCount > 0) {
    frameIndex = static_cast<size_t>(current_pose_frame_) % frameCount;
  }

  const auto texture = (*frames)[frameIndex];
  if (!texture)
  {
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

