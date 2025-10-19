#include "stdafx.h"
#include "Ui.h"
#include "System/Renderer/TextureWrapper.h"
#include "System/Renderer/Renderer.h"
#include "System/Renderer/Priority.h"

namespace UiConstants {
  const String kAirGaugeLeftImagePath = U"Assets/Image/Ui/barBlue_horizontalLeft.png";
  const String kAirGaugeCenterImagePath = U"Assets/Image/Ui/barBlue_horizontalBlue.png";
  const String kAirGaugeRightImagePath = U"Assets/Image/Ui/barBlue_horizontalRight.png";
  
  // 背景ゲージ用の画像パス
  const String kAirGaugeBackLeftImagePath = U"Assets/Image/Ui/barBack_horizontalLeft.png";
  const String kAirGaugeBackCenterImagePath = U"Assets/Image/Ui/barBack_horizontalMid.png";
  const String kAirGaugeBackRightImagePath = U"Assets/Image/Ui/barBack_horizontalRight.png";
}

/// <summary>
/// コンストラクタ
/// </summary>
Ui::Ui()
  : Task()
  , air_ratio_(1.0f)
  , gauge_x_(50)
  , gauge_y_(50)
  , center_width_(200)
{
  // 背景ゲージのテクスチャを読み込み
  air_gauge_back_left_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeBackLeftImagePath);
  air_gauge_back_center_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeBackCenterImagePath);
  air_gauge_back_right_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeBackRightImagePath);

  // エアゲージのテクスチャを読み込み
  air_gauge_left_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeLeftImagePath);
  air_gauge_center_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeCenterImagePath);
  air_gauge_right_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeRightImagePath);

  // 背景ゲージのTextureWrapperを初期化（最大値で固定）
  air_gauge_back_left_wrapper_ = std::make_shared<TextureWrapper>(air_gauge_back_left_texture_, gauge_x_, gauge_y_);
  air_gauge_back_center_wrapper_ = std::make_shared<TextureWrapper>(air_gauge_back_center_texture_, gauge_x_ + air_gauge_back_left_texture_->width(), gauge_y_);
  air_gauge_back_right_wrapper_ = std::make_shared<TextureWrapper>(air_gauge_back_right_texture_, gauge_x_ + air_gauge_back_left_texture_->width() + center_width_, gauge_y_);

  // エアゲージのTextureWrapperを初期化
  air_gauge_left_wrapper_ = std::make_shared<TextureWrapper>(air_gauge_left_texture_, gauge_x_, gauge_y_);
  air_gauge_center_wrapper_ = std::make_shared<TextureWrapper>(air_gauge_center_texture_, gauge_x_ + air_gauge_left_texture_->width(), gauge_y_);
  air_gauge_right_wrapper_ = std::make_shared<TextureWrapper>(air_gauge_right_texture_, gauge_x_ + air_gauge_left_texture_->width() + center_width_, gauge_y_);

  // 背景ゲージの中央部分をフルサイズでスケール設定
  float back_scale_x = static_cast<float>(center_width_) / static_cast<float>(air_gauge_back_center_texture_->width());
  air_gauge_back_center_wrapper_->SetScale(back_scale_x, 1.0f);
}

/// <summary>
/// デストラクタ
/// </summary>
Ui::~Ui()
{
}

/// <summary>
/// 毎フレーム更新処理
/// </summary>
/// <param name="delta_time">前回実行フレームからの経過時間（秒）</param>
void Ui::Update(float delta_time)
{
  // 背景ゲージの位置更新（常に最大値表示）
  if (air_gauge_back_left_wrapper_) {
    air_gauge_back_left_wrapper_->SetPosition(gauge_x_, gauge_y_);
  }

  if (air_gauge_back_center_wrapper_) {
    int back_center_x = gauge_x_ + air_gauge_back_left_texture_->width();
    air_gauge_back_center_wrapper_->SetPosition(back_center_x, gauge_y_);
    //air_gauge_back_center_wrapper_->SetPosition(gauge_x_, gauge_y_);
    // 背景は常にフルサイズで表示
    //air_gauge_center_wrapper_->SetScale(100, 1.0f);
  }

  if (air_gauge_back_right_wrapper_) {
    int back_right_x = gauge_x_ + air_gauge_back_left_texture_->width() + center_width_;
    air_gauge_back_right_wrapper_->SetPosition(back_right_x, gauge_y_);
  }

  // エアゲージの位置更新
  if (air_gauge_left_wrapper_) {
    air_gauge_left_wrapper_->SetPosition(gauge_x_, gauge_y_);
  }

  if (air_gauge_center_wrapper_) {
    int center_x = gauge_x_ + air_gauge_left_texture_->width();
    air_gauge_center_wrapper_->SetPosition(center_x, gauge_y_);

    // エア残量に応じて中央部分を横にスケール（Y軸は1.0で固定）
    float scale_x = (center_width_ * air_ratio_) / static_cast<float>(air_gauge_center_texture_->width());
    if (scale_x > 0.0f) {
      air_gauge_center_wrapper_->SetScale(scale_x, 1.0f);
    } else {
      air_gauge_center_wrapper_->SetScale(0.0f, 1.0f);
    }
  }

  if (air_gauge_right_wrapper_) {
    int right_x = gauge_x_ + air_gauge_left_texture_->width() + static_cast<int>(center_width_ * air_ratio_);
    air_gauge_right_wrapper_->SetPosition(right_x, gauge_y_);
  }
}

/// <summary>
/// 毎フレームの描画処理
/// </summary>
void Ui::Render()
{
  Renderer* renderer = Renderer::GetInstance();

  if (renderer) {
    // 背景ゲージを先に描画（後ろに表示）
    if (air_gauge_back_left_wrapper_) {
      renderer->Push(Priority::kSideUiPriority - 1, std::static_pointer_cast<Task>(air_gauge_back_left_wrapper_));
    }

    if (air_gauge_back_center_wrapper_) {
      renderer->Push(Priority::kSideUiPriority - 1, std::static_pointer_cast<Task>(air_gauge_back_center_wrapper_));
    }

    if (air_gauge_back_right_wrapper_) {
      renderer->Push(Priority::kSideUiPriority - 1, std::static_pointer_cast<Task>(air_gauge_back_right_wrapper_));
    }

    // エアゲージを描画（前面に表示）
    if (air_gauge_left_wrapper_) {
      renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(air_gauge_left_wrapper_));
    }

    // エア残量がある場合のみ中央部分を表示
    if (air_ratio_ > 0.0f && air_gauge_center_wrapper_) {
      renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(air_gauge_center_wrapper_));
    }

    if (air_gauge_right_wrapper_) {
      renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(air_gauge_right_wrapper_));
    }
  }
}

/// <summary>
/// エアゲージの値を設定（0.0〜1.0）
/// </summary>
/// <param name="air_ratio">エア残量の比率</param>
void Ui::SetAirGauge(float air_ratio)
{
  // 0.0〜1.0の範囲にクランプ
  air_ratio_ = Max(0.0f, Min(1.0f, air_ratio));
}

/// <summary>
/// エアゲージの位置を設定
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
void Ui::SetAirGaugePosition(int x, int y)
{
  gauge_x_ = x;
  gauge_y_ = y;
  
  // 背景ゲージの中央部分のスケールを再計算
  if (air_gauge_back_center_wrapper_) {
    float back_scale_x = static_cast<float>(center_width_) / static_cast<float>(air_gauge_back_center_texture_->width());
    air_gauge_back_center_wrapper_->SetScale(back_scale_x, 1.0f);
  }
}
