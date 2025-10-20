#include "System/Renderer/TextureWrapper.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="texture">テクスチャ</param>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
TextureWrapper::TextureWrapper(std::shared_ptr<Texture> texture, int x, int y)
  : texture_(texture)
  , x_(x)
  , y_(y)
  , uv_rect_({0,0,0,0}) {  // UV座標なし
}

/// <summary>
/// UV指定コンストラクタ
/// </summary>
/// <param name="texture">テクスチャ</param>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
/// <param name="uvRect">UV座標</param>
TextureWrapper::TextureWrapper(std::shared_ptr<Texture> texture, int x, int y, const Rect& uvRect)
  : texture_(texture)
  , x_(x)
  , y_(y)
  , uv_rect_(uvRect) {  // UV座標あり
}

/// <summary>
/// デストラクタ
/// </summary>
TextureWrapper::~TextureWrapper() {
}

/// <summary>
/// 位置を設定
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
void TextureWrapper::SetPosition(int x, int y) {
  x_ = x;
  y_ = y;
}

/// <summary>
/// UV座標を設定
/// </summary>
/// <param name="uvRect">UV座標</param>
void TextureWrapper::SetUVRect(const Rect& uvRect) {
  uv_rect_ = uvRect;
}

/// <summary>
/// 位置を取得
/// </summary>
/// <returns>位置</returns>
Point TextureWrapper::GetPosition() const {
  return Point(x_, y_);
}

/// <summary>
/// テクスチャを取得
/// </summary>
/// <returns>テクスチャ</returns>
std::shared_ptr<Texture> TextureWrapper::GetTexture() const {
  return texture_;
}

/// <summary>
/// 毎フレーム更新処理
/// </summary>
/// <param name="delta_time">前回実行フレームからの経過時間（秒）</param>
void TextureWrapper::Update(float delta_time) {
  // 必要に応じて位置の更新処理を追加
}

/// <summary>
/// 毎フレームの描画処理
/// </summary>
void TextureWrapper::Render() {
  if (texture_) {
    if (uv_rect_) {
      if (is_center_) {
      // UV座標指定あり（スプライトシート）
        texture_->operator()(uv_rect_).scaled(scale_x_, scale_y_).drawAt(x_, y_, color_);
      } else {
        texture_->operator()(uv_rect_).scaled(scale_x_, scale_y_).draw(x_, y_, color_);
      }
      //texture_->draw(x_, y_);
    }
    else {
      // 通常描画
      if (is_center_) {
        texture_->scaled(scale_x_, scale_y_).drawAt(x_, y_, color_);
      } else {
        texture_->scaled(scale_x_, scale_y_).draw(x_, y_, color_);
      }
    }
  }
}

// SetColorのconst修飾子を削除
void TextureWrapper::SetColor(ColorF color) {
  color_ = color;
}

void TextureWrapper::SetScale(float scale) {
  scale_ = scale;
  scale_x_ = scale;
  scale_y_ = scale;
}

/// <summary>
/// X軸とY軸のスケールを個別に設定
/// </summary>
/// <param name="scale_x">X軸のスケール</param>
/// <param name="scale_y">Y軸のスケール</param>
void TextureWrapper::SetScale(float scale_x, float scale_y) {
  scale_x_ = scale_x;
  scale_y_ = scale_y;
  scale_ = scale_x; // 互換性のため
}

float TextureWrapper::GetScale() const {
  return scale_;
}

void TextureWrapper::SetIsCenter(bool is_center) {
  is_center_ = is_center;
}

Rect TextureWrapper::GetUvRect() const {
  return uv_rect_;
}
