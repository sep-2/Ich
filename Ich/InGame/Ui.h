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

    /// <summary>
    /// サイドボックスの表示/非表示を設定
    /// </summary>
    /// <param name="visible">表示する場合 true</param>
    void SetSideBoxVisible(bool visible);

    /// <summary>
    /// サイドボックスの位置を設定
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    void SetSideBoxPosition(int x, int y);

    /// <summary>
    /// サイドボックスのサイズを設定
    /// </summary>
    /// <param name="width">幅</param>
    /// <param name="height">高さ</param>
    void SetSideBoxSize(int width, int height);

private:
    /// <summary>
    /// Nine-Patch用のTextureWrapperを初期化
    /// </summary>
    void InitializeNinePatchWrappers();

    /// <summary>
    /// Nine-Patchの位置を更新
    /// </summary>
    void UpdateNinePatchPositions();

    /// <summary>
    /// Nine-Patchのスケールを更新
    /// </summary>
    void UpdateNinePatchScales();

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
    /// サイドボックス（インフォメーション表示用）のテクスチャ
    /// </summary>
    std::shared_ptr<Texture> side_box_texture_;

    /// <summary>
    /// サイドボックス表示用ラッパー
    /// </summary>
    std::shared_ptr<TextureWrapper> side_box_wrapper_;

    // Nine-Patch用のサイドボックス要素
    /// <summary>
    /// サイドボックスの左上コーナー
    /// </summary>
    std::shared_ptr<Texture> side_box_top_left_texture_;
    std::shared_ptr<TextureWrapper> side_box_top_left_wrapper_;

    /// <summary>
    /// サイドボックスの上部中央（水平方向に伸縮）
    /// </summary>
    std::shared_ptr<Texture> side_box_top_center_texture_;
    std::shared_ptr<TextureWrapper> side_box_top_center_wrapper_;

    /// <summary>
    /// サイドボックスの右上コーナー
    /// </summary>
    std::shared_ptr<Texture> side_box_top_right_texture_;
    std::shared_ptr<TextureWrapper> side_box_top_right_wrapper_;

    /// <summary>
    /// サイドボックスの左部中央（垂直方向に伸縮）
    /// </summary>
    std::shared_ptr<Texture> side_box_left_center_texture_;
    std::shared_ptr<TextureWrapper> side_box_left_center_wrapper_;

    /// <summary>
    /// サイドボックスの中央部分（両方向に伸縮）
    /// </summary>
    std::shared_ptr<Texture> side_box_center_texture_;
    std::shared_ptr<TextureWrapper> side_box_center_wrapper_;

    /// <summary>
    /// サイドボックスの右部中央（垂直方向に伸縮）
    /// </summary>
    std::shared_ptr<Texture> side_box_right_center_texture_;
    std::shared_ptr<TextureWrapper> side_box_right_center_wrapper_;

    /// <summary>
    /// サイドボックスの左下コーナー
    /// </summary>
    std::shared_ptr<Texture> side_box_bottom_left_texture_;
    std::shared_ptr<TextureWrapper> side_box_bottom_left_wrapper_;

    /// <summary>
    /// サイドボックスの下部中央（水平方向に伸縮）
    /// </summary>
    std::shared_ptr<Texture> side_box_bottom_center_texture_;
    std::shared_ptr<TextureWrapper> side_box_bottom_center_wrapper_;

    /// <summary>
    /// サイドボックスの右下コーナー
    /// </summary>
    std::shared_ptr<Texture> side_box_bottom_right_texture_;
    std::shared_ptr<TextureWrapper> side_box_bottom_right_wrapper_;

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

    /// <summary>
    /// サイドボックスの表示位置X
    /// </summary>
    int side_box_x_;

    /// <summary>
    /// サイドボックスの表示位置Y
    /// </summary>
    int side_box_y_;

    /// <summary>
    /// サイドボックスの表示フラグ
    /// </summary>
    bool side_box_visible_;

    /// <summary>
    /// サイドボックスの拡大後の幅
    /// </summary>
    int side_box_width_;

    /// <summary>
    /// サイドボックスの拡大後の高さ
    /// </summary>
    int side_box_height_;

    /// <summary>
    /// サイドボックスの枠の厚さ（Nine-Patch用）
    /// </summary>
    int side_box_border_size_;
};

