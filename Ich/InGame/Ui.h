#pragma once
#include "System/Task/Task.h"
#include <memory>
#include <Siv3D.hpp>

class TextureWrapper;

/// <summary>
/// UIクラス - エアゲージなどのゲーム内UIを管理
/// </summary>
class Ui : public Task
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Ui();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~Ui();

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
    /// エアゲージの値を設定（0.0〜1.0）
    /// </summary>
    /// <param name="air_ratio">エア残量の比率</param>
    void SetAirGauge(float air_ratio);

    /// <summary>
    /// エアゲージの位置を設定
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    void SetAirGaugePosition(int x, int y);

private:
    /// <summary>
    /// エアゲージ背景のテクスチャ（左端）
    /// </summary>
    std::shared_ptr<Texture> air_gauge_back_left_texture_;

    /// <summary>
    /// エアゲージ背景のテクスチャ（中央部分）
    /// </summary>
    std::shared_ptr<Texture> air_gauge_back_center_texture_;

    /// <summary>
    /// エアゲージ背景のテクスチャ（右端）
    /// </summary>
    std::shared_ptr<Texture> air_gauge_back_right_texture_;

    /// <summary>
    /// エアゲージ背景の左端表示用ラッパー
    /// </summary>
    std::shared_ptr<TextureWrapper> air_gauge_back_left_wrapper_;

    /// <summary>
    /// エアゲージ背景の中央部分表示用ラッパー
    /// </summary>
    std::shared_ptr<TextureWrapper> air_gauge_back_center_wrapper_;

    /// <summary>
    /// エアゲージ背景の右端表示用ラッパー
    /// </summary>
    std::shared_ptr<TextureWrapper> air_gauge_back_right_wrapper_;

    /// <summary>
    /// エアゲージのテクスチャ（左端）
    /// </summary>
    std::shared_ptr<Texture> air_gauge_left_texture_;

    /// <summary>
    /// エアゲージのテクスチャ（中央部分）
    /// </summary>
    std::shared_ptr<Texture> air_gauge_center_texture_;

    /// <summary>
    /// エアゲージのテクスチャ（右端）
    /// </summary>
    std::shared_ptr<Texture> air_gauge_right_texture_;

    /// <summary>
    /// エアゲージの左端表示用ラッパー
    /// </summary>
    std::shared_ptr<TextureWrapper> air_gauge_left_wrapper_;

    /// <summary>
    /// エアゲージの中央部分表示用ラッパー
    /// </summary>
    std::shared_ptr<TextureWrapper> air_gauge_center_wrapper_;

    /// <summary>
    /// エアゲージの右端表示用ラッパー
    /// </summary>
    std::shared_ptr<TextureWrapper> air_gauge_right_wrapper_;

    /// <summary>
    /// エアゲージの現在値（0.0〜1.0）
    /// </summary>
    float air_ratio_;

    /// <summary>
    /// エアゲージの表示位置X
    /// </summary>
    int gauge_x_;

    /// <summary>
    /// エアゲージの表示位置Y
    /// </summary>
    int gauge_y_;

    /// <summary>
    /// エアゲージの中央部分の幅
    /// </summary>
    int center_width_;
};

