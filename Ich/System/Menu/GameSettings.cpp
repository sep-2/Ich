#include "stdafx.h"
#include "GameSettings.h"

std::shared_ptr<GameSettings> GameSettings::instance_ = nullptr;

GameSettings* GameSettings::GetInstance() {
  if (instance_ == nullptr) {
    instance_ = std::make_shared<GameSettings>();
  }
  return instance_.get();
}

GameSettings::GameSettings() {
  Load();
  // 読み込んだ設定を即座に適用
  SetVolume(volume_);
}

GameSettings::~GameSettings() {
  Save();
}

void GameSettings::Save() {
  JSON json;
  json[U"volume"] = volume_;
  json[U"brightness"] = brightness_;
  json.save(kSettingsFilePath);
}

void GameSettings::Load() {
  const JSON json = JSON::Load(kSettingsFilePath);
  if (json) {
    volume_ = json[U"volume"].get<double>();
    brightness_ = json[U"brightness"].get<double>();
  }
}

void GameSettings::SetVolume(double volume) {
  volume_ = Clamp(volume, 0.0, 1.0);
  // 音量を即座に適用
  GlobalAudio::SetVolume(volume_);
}

void GameSettings::SetBrightness(double brightness) {
  brightness_ = Clamp(brightness, 0.0, 1.0);
  // 明るさは描画時に ApplyBrightness() で適用される
}

void GameSettings::ApplyBrightness() const {
  // 画面全体に明るさ調整用の半透明レイヤーを描画
  if (brightness_ < 1.0) {
    const double darkness = 1.0 - brightness_;
    Scene::Rect().draw(ColorF(0.0, 0.0, 0.0, darkness));
  }
}
