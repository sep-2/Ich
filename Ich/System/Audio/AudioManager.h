#pragma once
#include <map>
#include <string>
#include <memory>

#include <Siv3D.hpp>

/// <summary>
/// 効果音の種類
/// </summary>
enum class SeKind {
  kDecideSe,
  kChangeSceneSe,
};

/// <summary>
/// BGMの種類
/// </summary>
enum class BgmKind {
};

/// <summary>
/// オーディオマネージャ
/// </summary>
class AudioManager {
private:
  static inline const std::map<SeKind, std::string> kSeFilepaths = {
    {SeKind::kDecideSe, "Assets/Audio/Decide.mp3"},
    {SeKind::kChangeSceneSe, "Assets/Audio/Change.mp3"},
  };

  static inline const std::map<BgmKind, std::string> kBgmFilepaths = {
  };

public:
  /// <summary>
  /// シングルトンインスタンス取得
  /// </summary>
  static AudioManager* GetInstance() {
    if (instance_ == nullptr) {
      instance_ = std::make_shared<AudioManager>();
    }
    return instance_.get();
  }

  /// <summary>
  /// シングルトンインスタンス解放（終了時）
  /// </summary>
  static void Destroy() {
    if (instance_ != nullptr) {
      // 停止してから破棄
      for (auto& handle : instance_->bgm_) {
        handle.second.stop();
      }
      for (auto& handle : instance_->se_) {
        handle.second.stop();
      }
      instance_ = nullptr;
    }
  }

public:
  AudioManager();
  ~AudioManager();

  void InitAudioManager();
  void FinAudioManager();

  void LoadSe(SeKind kind);
  void PlaySe(SeKind kind);
  void DeleteSe(SeKind kind);

  void LoadBgm(BgmKind kind);
  void PlayBgm(BgmKind kind);
  void DeleteBgm(BgmKind kind);

  void StopBgm();
  void FadeOutBgm();
  void FadeInBgm();

private:
  /// <summary>
  /// シングルトン用インスタンス
  /// </summary>
  static std::shared_ptr<AudioManager> instance_;

  // DxLib の int ハンドルを Siv3D の Audio に置換
  std::map<SeKind, s3d::Audio> se_;
  std::map<BgmKind, s3d::Audio> bgm_;
};

