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
  
  // サイドボックス用の画像パス
  const String kSideBoxImagePath = U"Assets/Image/Ui/SideBox.png";
}

/// <summary>
/// コンストラクタ
/// </summary>
Ui::Ui()
  : Task()
  , air_ratio_(1.0f)
  , gauge_x_(500)
  , gauge_y_(50)
  , center_width_(200)
  , side_box_x_(480)
  , side_box_y_(100)
  , side_box_visible_(true)
  , side_box_width_(300)
  , side_box_height_(400)
  , side_box_border_size_(16)  // 枠の厚さを16ピクセルと仮定
{
  // 背景ゲージのテクスチャを読み込み
  air_gauge_back_left_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeBackLeftImagePath);
  air_gauge_back_center_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeBackCenterImagePath);
  air_gauge_back_right_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeBackRightImagePath);

  // エアゲージのテクスチャを読み込み
  air_gauge_left_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeLeftImagePath);
  air_gauge_center_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeCenterImagePath);
  air_gauge_right_texture_ = std::make_shared<Texture>(UiConstants::kAirGaugeRightImagePath);

  // サイドボックスの元画像を読み込み、Nine-Patchで分割
  Image side_box_image = Image(UiConstants::kSideBoxImagePath);
  int border = side_box_border_size_;
  int original_width = side_box_image.width();
  int original_height = side_box_image.height();

  // 9つの部分に分割してテクスチャを作成
  // 左上コーナー
  side_box_top_left_texture_ = std::make_shared<Texture>(side_box_image.clipped(0, 0, border, border));
  
  // 上部中央（水平方向に伸縮）
  side_box_top_center_texture_ = std::make_shared<Texture>(side_box_image.clipped(border, 0, original_width - border * 2, border));
  
  // 右上コーナー
  side_box_top_right_texture_ = std::make_shared<Texture>(side_box_image.clipped(original_width - border, 0, border, border));
  
  // 左部中央（垂直方向に伸縮）
  side_box_left_center_texture_ = std::make_shared<Texture>(side_box_image.clipped(0, border, border, original_height - border * 2));
  
  // 中央部分（両方向に伸縮）
  side_box_center_texture_ = std::make_shared<Texture>(side_box_image.clipped(border, border, original_width - border * 2, original_height - border * 2));
  
  // 右部中央（垂直方向に伸縮）
  side_box_right_center_texture_ = std::make_shared<Texture>(side_box_image.clipped(original_width - border, border, border, original_height - border * 2));
  
  // 左下コーナー
  side_box_bottom_left_texture_ = std::make_shared<Texture>(side_box_image.clipped(0, original_height - border, border, border));
  
  // 下部中央（水平方向に伸縮）
  side_box_bottom_center_texture_ = std::make_shared<Texture>(side_box_image.clipped(border, original_height - border, original_width - border * 2, border));
  
  // 右下コーナー
  side_box_bottom_right_texture_ = std::make_shared<Texture>(side_box_image.clipped(original_width - border, original_height - border, border, border));

  // Nine-Patch用のTextureWrapperを初期化
  InitializeNinePatchWrappers();

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
  }

  if (air_gauge_back_right_wrapper_) {
    int back_right_x = gauge_x_ + air_gauge_back_left_texture_->width() + center_width_;
    air_gauge_back_right_wrapper_->SetPosition(back_right_x, gauge_y_);
  }

  // 左端のスケール計算
  float left_scale = 1.0f;
  if (air_ratio_ < 0.1f) {  // 10%以下になったら左端も縮小開始
    left_scale = air_ratio_ / 0.1f;  // 10%で1.0、0%で0.0
  }

  // 右端のスケール計算
  float right_scale = 1.0f;
  if (air_ratio_ < 0.1f) {  // 10%以下になったら右端も縮小開始
    right_scale = air_ratio_ / 0.1f;  // 10%で1.0、0%で0.0
  }

  // エアゲージの位置更新
  if (air_gauge_left_wrapper_) {
    air_gauge_left_wrapper_->SetPosition(gauge_x_, gauge_y_);
    air_gauge_left_wrapper_->SetScale(left_scale, 1.0f);
  }

  if (air_gauge_center_wrapper_) {
    // 左端の縮小を考慮した中央部分の開始位置
    int left_width_scaled = static_cast<int>(air_gauge_left_texture_->width() * left_scale);
    int center_x = gauge_x_ + left_width_scaled;
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
    // 左端の縮小と中央部分の幅を考慮した右端の位置
    int left_width_scaled = static_cast<int>(air_gauge_left_texture_->width() * left_scale);
    int center_width_scaled = static_cast<int>(center_width_ * air_ratio_);
    int right_x = gauge_x_ + left_width_scaled + center_width_scaled;
    
    air_gauge_right_wrapper_->SetPosition(right_x, gauge_y_);
    air_gauge_right_wrapper_->SetScale(right_scale, 1.0f);
  }

  // Nine-Patchサイドボックスの位置更新
  UpdateNinePatchPositions();
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
    // エア残量がある場合のみ左端を表示
    if (air_ratio_ > 0.0f && air_gauge_left_wrapper_) {
      renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(air_gauge_left_wrapper_));
    }

    // エア残量がある場合のみ中央部分を表示
    if (air_ratio_ > 0.0f && air_gauge_center_wrapper_) {
      renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(air_gauge_center_wrapper_));
    }

    // エア残量がある場合のみ右端を表示
    if (air_ratio_ > 0.0f && air_gauge_right_wrapper_) {
      renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(air_gauge_right_wrapper_));
    }

    // Nine-Patchサイドボックスを描画（インフォメーション表示用）
    if (side_box_visible_) {
      // 9つの部分を順番に描画
      if (side_box_top_left_wrapper_) renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(side_box_top_left_wrapper_));
      if (side_box_top_center_wrapper_) renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(side_box_top_center_wrapper_));
      if (side_box_top_right_wrapper_) renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(side_box_top_right_wrapper_));
      
      if (side_box_left_center_wrapper_) renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(side_box_left_center_wrapper_));
      if (side_box_center_wrapper_) renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(side_box_center_wrapper_));
      if (side_box_right_center_wrapper_) renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(side_box_right_center_wrapper_));
      
      if (side_box_bottom_left_wrapper_) renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(side_box_bottom_left_wrapper_));
      if (side_box_bottom_center_wrapper_) renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(side_box_bottom_center_wrapper_));
      if (side_box_bottom_right_wrapper_) renderer->Push(Priority::kSideUiPriority, std::static_pointer_cast<Task>(side_box_bottom_right_wrapper_));
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

/// <summary>
/// サイドボックスの表示/非表示を設定
/// </summary>
/// <param name="visible">表示する場合 true</param>
void Ui::SetSideBoxVisible(bool visible)
{
  side_box_visible_ = visible;
}

/// <summary>
/// サイドボックスの位置を設定
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
void Ui::SetSideBoxPosition(int x, int y)
{
  side_box_x_ = x;
  side_box_y_ = y;
  
  // Nine-Patchラッパーの位置を更新
  UpdateNinePatchPositions();
}

/// <summary>
/// サイドボックスのサイズを設定
/// </summary>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
void Ui::SetSideBoxSize(int width, int height)
{
  side_box_width_ = width;
  side_box_height_ = height;
  
  // サイズ変更後にスケールと位置を更新
  UpdateNinePatchScales();
  UpdateNinePatchPositions();
}

/// <summary>
/// Nine-Patch用のTextureWrapperを初期化
/// </summary>
void Ui::InitializeNinePatchWrappers()
{
  int border = side_box_border_size_;
  
  // 各部分のTextureWrapperを作成
  side_box_top_left_wrapper_ = std::make_shared<TextureWrapper>(side_box_top_left_texture_, side_box_x_, side_box_y_);
  side_box_top_center_wrapper_ = std::make_shared<TextureWrapper>(side_box_top_center_texture_, side_box_x_ + border, side_box_y_);
  side_box_top_right_wrapper_ = std::make_shared<TextureWrapper>(side_box_top_right_texture_, side_box_x_ + side_box_width_ - border, side_box_y_);
  
  side_box_left_center_wrapper_ = std::make_shared<TextureWrapper>(side_box_left_center_texture_, side_box_x_, side_box_y_ + border);
  side_box_center_wrapper_ = std::make_shared<TextureWrapper>(side_box_center_texture_, side_box_x_ + border, side_box_y_ + border);
  side_box_right_center_wrapper_ = std::make_shared<TextureWrapper>(side_box_right_center_texture_, side_box_x_ + side_box_width_ - border, side_box_y_ + border);
  
  side_box_bottom_left_wrapper_ = std::make_shared<TextureWrapper>(side_box_bottom_left_texture_, side_box_x_, side_box_y_ + side_box_height_ - border);
  side_box_bottom_center_wrapper_ = std::make_shared<TextureWrapper>(side_box_bottom_center_texture_, side_box_x_ + border, side_box_y_ + side_box_height_ - border);
  side_box_bottom_right_wrapper_ = std::make_shared<TextureWrapper>(side_box_bottom_right_texture_, side_box_x_ + side_box_width_ - border, side_box_y_ + side_box_height_ - border);
  
  // スケールを設定
  UpdateNinePatchScales();
}

/// <summary>
/// Nine-Patchの位置を更新
/// </summary>
void Ui::UpdateNinePatchPositions()
{
  int border = side_box_border_size_;
  
  if (side_box_top_left_wrapper_) side_box_top_left_wrapper_->SetPosition(side_box_x_, side_box_y_);
  if (side_box_top_center_wrapper_) side_box_top_center_wrapper_->SetPosition(side_box_x_ + border, side_box_y_);
  if (side_box_top_right_wrapper_) side_box_top_right_wrapper_->SetPosition(side_box_x_ + side_box_width_ - border, side_box_y_);
  
  if (side_box_left_center_wrapper_) side_box_left_center_wrapper_->SetPosition(side_box_x_, side_box_y_ + border);
  if (side_box_center_wrapper_) side_box_center_wrapper_->SetPosition(side_box_x_ + border, side_box_y_ + border);
  if (side_box_right_center_wrapper_) side_box_right_center_wrapper_->SetPosition(side_box_x_ + side_box_width_ - border, side_box_y_ + border);
  
  if (side_box_bottom_left_wrapper_) side_box_bottom_left_wrapper_->SetPosition(side_box_x_, side_box_y_ + side_box_height_ - border);
  if (side_box_bottom_center_wrapper_) side_box_bottom_center_wrapper_->SetPosition(side_box_x_ + border, side_box_y_ + side_box_height_ - border);
  if (side_box_bottom_right_wrapper_) side_box_bottom_right_wrapper_->SetPosition(side_box_x_ + side_box_width_ - border, side_box_y_ + side_box_height_ - border);
}

/// <summary>
/// Nine-Patchのスケールを更新
/// </summary>
void Ui::UpdateNinePatchScales()
{
  int border = side_box_border_size_;
  
  // 水平方向の伸縮倍率
  float horizontal_scale = static_cast<float>(side_box_width_ - border * 2) / static_cast<float>(side_box_top_center_texture_->width());
  
  // 垂直方向の伸縮倍率
  float vertical_scale = static_cast<float>(side_box_height_ - border * 2) / static_cast<float>(side_box_left_center_texture_->height());
  
  // コーナー部分はスケールしない
  if (side_box_top_left_wrapper_) side_box_top_left_wrapper_->SetScale(1.0f, 1.0f);
  if (side_box_top_right_wrapper_) side_box_top_right_wrapper_->SetScale(1.0f, 1.0f);
  if (side_box_bottom_left_wrapper_) side_box_bottom_left_wrapper_->SetScale(1.0f, 1.0f);
  if (side_box_bottom_right_wrapper_) side_box_bottom_right_wrapper_->SetScale(1.0f, 1.0f);
  
  // 水平方向の辺は横方向のみスケール
  if (side_box_top_center_wrapper_) side_box_top_center_wrapper_->SetScale(horizontal_scale, 1.0f);
  if (side_box_bottom_center_wrapper_) side_box_bottom_center_wrapper_->SetScale(horizontal_scale, 1.0f);
  
  // 垂直方向の辺は縦方向のみスケール
  if (side_box_left_center_wrapper_) side_box_left_center_wrapper_->SetScale(1.0f, vertical_scale);
  if (side_box_right_center_wrapper_) side_box_right_center_wrapper_->SetScale(1.0f, vertical_scale);
  
  // 中央部分は両方向にスケール
  if (side_box_center_wrapper_) side_box_center_wrapper_->SetScale(horizontal_scale, vertical_scale);
}
