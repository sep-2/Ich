#include <Siv3D.hpp>

#include "System/Audio/AudioManager.h"

/// <summary>
/// 無名名前空間
/// </summary>
namespace {
  /// <summary>
  /// ボリューム
  /// </summary>
  const int kVolumePal = 128;

  /// <summary>
  /// Maxボリューム
  /// </summary>
  const int kVolumePalMax = 255;

  /// <summary>
  /// Fadeスピード
  /// </summary>
  const int kFadeSpeed = 30;
}

// シングルトン実体
std::shared_ptr<AudioManager> AudioManager::instance_ = nullptr;

/// <summary>
/// コンストラクタ
/// </summary>
AudioManager::AudioManager()
  : se_()
  , bgm_() {

}

/// <summary>
/// デストラクタ
/// </summary>
AudioManager::~AudioManager() {

}

/// <summary>
/// 初期処理
/// </summary>
void AudioManager::InitAudioManager() {

}

/// <summary>
/// 後処理
/// </summary>
void AudioManager::FinAudioManager() {

}

/// <summary>
/// 効果音の読み込み
/// </summary>
void AudioManager::LoadSe(SeKind kind) {
  //LoadSoundMem(kSeFilepaths.at(kind).c_str());
  if (se_.count(kind) > 0) {
    return;
  }
  se_.insert(std::make_pair(kind, s3d::Audio(s3d::Unicode::FromUTF8(kSeFilepaths.at(kind)))));
}

/// <summary>
/// 効果音の再生
/// </summary>
void AudioManager::PlaySe(SeKind kind) {
  LoadSe(kind);

  se_.at(kind).setVolume(1.0);
  se_.at(kind).playOneShot(5.0);
}

/// <summary>
/// 効果音の破棄
/// </summary>
void AudioManager::DeleteSe(SeKind kind) {
  se_.at(kind).stop();
  se_.erase(kind);
}

/// <summary>
/// BGMの読み込み
/// </summary>
void AudioManager::LoadBgm(BgmKind kind) {
  if (bgm_.count(kind) > 0) {
    return;
  }
  bgm_.insert(std::make_pair(kind, s3d::Audio(s3d::Unicode::FromUTF8(kBgmFilepaths.at(kind)))));
}

/// <summary>
/// BGMの再生
/// </summary>
void AudioManager::PlayBgm(BgmKind kind) {
  LoadBgm(kind);
  bgm_.at(kind).setVolume(1.0);
  bgm_.at(kind).setLoop(true);
  bgm_.at(kind).play();
}

/// <summary>
/// BGMの破棄
/// </summary>
void AudioManager::DeleteBgm(BgmKind kind) {
  bgm_.at(kind).stop();
  bgm_.erase(kind);
}

/// <summary>
/// BGMの一時停止機能
/// </summary>
void AudioManager::StopBgm() {
  for (auto& handle : bgm_) {
    handle.second.stop();
  }
}

/// <summary>
/// BGMのフェードアウト
/// </summary>
void AudioManager::FadeOutBgm() {
  //for (std::pair<BgmKind, s3d::Audio> handle : bgm_) {
  //  double volume = handle.second.volume();
  //  volume -= kFadeSpeed * 0.01;
  //  if (volume < 0) {
  //    volume = 0;
  //  }
  //  handle.second.setVolume(volume);
  //}
}

/// <summary>
/// BGMのフェードイン
/// </summary>
void AudioManager::FadeInBgm() {
  //for (std::pair<BgmKind, s3d::Audio> handle : bgm_) {
  //  double volume = handle.second.volume();
  //  volume += kFadeSpeed * 0.01;
  //  if (volume > kVolumePal) {
  //    volume = kVolumePal;
  //  }
  //  handle.second.setVolume(volume);
  //}
}
