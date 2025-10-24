#pragma once

#include <memory>
#include <Siv3D.hpp>
#include "System/Task/Task.h"

/// <summary>
/// ポジション付きテクスチャラッパークラス
/// </summary>
class TextureWrapper : public Task {

public:
  /// <summary>
  /// コンストラクタ
  /// </summary>
  /// <param name="texture">テクスチャ</param>
  /// <param name="x">X座標</param>
  /// <param name="y">Y座標</param>
  TextureWrapper(std::shared_ptr<Texture> texture, int x, int y);

  /// <summary>
  /// UV指定コンストラクタ
  /// </summary>
  /// <param name="texture">テクスチャ</param>
  /// <param name="x">X座標</param>
  /// <param name="y">Y座標</param>
  /// <param name="uvRect">UV座標</param>
  TextureWrapper(std::shared_ptr<Texture> texture, int x, int y, const Rect& uv_rect);

  /// <summary>
  /// デストラクタ
  /// </summary>
  virtual ~TextureWrapper();

  /// <summary>
  /// 位置を設定
  /// </summary>
  /// <param name="x">X座標</param>
  /// <param name="y">Y座標</param>
  void SetPosition(int x, int y);

  /// <summary>
  /// UV座標を設定
  /// </summary>
  /// <param name="uvRect">UV座標</param>
  void SetUVRect(const Rect& uv_rect_);

  /// <summary>
  /// 位置を取得
  /// </summary>
  /// <returns>位置</returns>
  Point GetPosition() const;

  /// <summary>
  /// テクスチャを取得
  /// </summary>
  /// <returns>テクスチャ</returns>
  std::shared_ptr<Texture> GetTexture() const;

  /// <summary>
  /// テクスチャを差し替える
  /// </summary>
  /// <param name="texture">新しいテクスチャ</param>
  void SetTexture(std::shared_ptr<Texture> texture);

  void SetColor(ColorF color);

  /// <summary>
  /// 毎フレーム更新処理
  /// </summary>
  /// <param name="delta_time">前回実行フレームからの経過時間（秒）</param>
  void Update(float delta_time) override;

  /// <summary>
  /// 毎フレームの描画処理
  /// </summary>
  void Render() override;

  void SetScale(float scale);

  /// <summary>
  /// X軸とY軸のスケールを個別に設定
  /// </summary>
  /// <param name="scale_x">X軸のスケール</param>
  /// <param name="scale_y">Y軸のスケール</param>
  void SetScale(float scale_x, float scale_y);

  float GetScale() const;
  float GetScaleX() const;
  float GetScaleY() const;

  void SetIsCenter(bool is_center);

  Rect GetUvRect() const;

private:
  /// <summary>
  /// テクスチャ
  /// </summary>
  std::shared_ptr<Texture> texture_;

  /// <summary>
  /// X座標
  /// </summary>
  int x_;

  /// <summary>
  /// Y座標
  /// </summary>
  int y_;

  /// <summary>
  /// UV座標（スプライトシート用）
  /// </summary>
  Rect uv_rect_;

  ColorF color_ = ColorF{1};

  float scale_ = 1.0f;

  /// <summary>
  /// X軸のスケール
  /// </summary>
  float scale_x_ = 1.0f;

  /// <summary>
  /// Y軸のスケール
  /// </summary>
  float scale_y_ = 1.0f;

  bool is_center_ = false;
};
