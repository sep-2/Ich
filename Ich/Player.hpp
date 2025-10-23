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
    /// <summary>
    /// プレイヤーのポーズを表す列挙体
    /// </summary>
    enum class Pose
    {
        kIdle,               ///< 待機
        kStrafeLeft,         ///< 左向き移動（横移動）
        kStrafeRight,        ///< 右向き移動（横移動）
        kWalkForwardLeft,    ///< 前向き歩行（左側リード）
        kWalkForwardRight,   ///< 前向き歩行（右側リード）
        kFall,               ///< 落下
        kGameOver            ///< ゲームオーバー演出
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

    /// <summary>
    /// 現在のポーズを取得
    /// </summary>
    /// <returns>プレイヤーのポーズ</returns>
    Pose GetPose() const;

    /// <summary>
    /// 現在のポーズを設定
    /// </summary>
    /// <param name="pose">設定するポーズ</param>
    void SetPose(Pose pose);

    /// <summary>
    /// 移動フラグと向き情報からポーズを再評価する
    /// （落下・ゲームオーバーなどの特別ポーズ解除時に利用）
    /// </summary>
    void RefreshPoseFromMovement();

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
    /// プレイヤー表示用ラッパー
    /// </summary>
    std::shared_ptr<TextureWrapper> player_wrapper_;

    /// <summary>
    /// プレイヤーの位置
    /// </summary>
    Vec2 position_;

    /// <summary>
    /// プレイヤーの移動速度（ピクセル/秒）
    /// </summary>
    float move_speed_;

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
    /// プレイヤーのポーズ
    /// </summary>
    Pose pose_;

    /// <summary>
    /// ポーズごとのテクスチャを読み込む
    /// </summary>
    void LoadPoseTextures();

    /// <summary>
    /// 指定したポーズに対応するテクスチャを取得
    /// </summary>
    std::shared_ptr<Texture> GetTextureForPose(Pose pose) const;

    /// <summary>
    /// 現在のポーズに応じてテクスチャとスケールを更新
    /// </summary>
    void UpdateTextureForPose();

    /// <summary>
    /// ポーズごとのテクスチャキャッシュ
    /// </summary>
    HashTable<Pose, std::shared_ptr<Texture>> pose_textures_;

    /// <summary>
    /// 移動フラグから算出される基本ポーズを取得
    /// </summary>
    Pose CalculateMovementPose() const;

    /// <summary>
    /// 移動フラグ由来のポーズを適用
    /// </summary>
    /// <param name="force">特殊ポーズ中でも更新するか</param>
    void ApplyPoseFromMovement(bool force);

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
    /// <summary>
    /// 最終的に描画したい概ねの高さ（ピクセル）。ポーズによって画像サイズが異なるので、ここを基準にリサイズする。
    /// </summary>
    static constexpr float kTargetHeight = 90.0f;
};
