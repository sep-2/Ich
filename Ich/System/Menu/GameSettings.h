#pragma once
#include <memory>
#include <Siv3D.hpp>

/// <summary>
/// ゲーム設定を管理するシングルトンクラス
/// </summary>
class GameSettings {
public:
  static GameSettings* GetInstance();

  static void Destroy() {
    if (instance_ != nullptr) {
      instance_ = nullptr;
    }
  }

  GameSettings(const GameSettings&) = delete;
  GameSettings& operator=(const GameSettings&) = delete;

  /// <summary>
  /// 設定を保存
  /// </summary>
  void Save();

  /// <summary>
  /// 設定を読み込み
  /// </summary>
  void Load();

  /// <summary>
  /// 音量を設定 (0.0 ~ 1.0)
  /// </summary>
  void SetVolume(double volume);

  /// <summary>
  /// 音量を取得
  /// </summary>
  double GetVolume() const { return volume_; }

  /// <summary>
  /// 画面の明るさを設定 (0.0 ~ 1.0)
  /// </summary>
  void SetBrightness(double brightness);

  /// <summary>
  /// 画面の明るさを取得
  /// </summary>
  double GetBrightness() const { return brightness_; }

  /// <summary>
  /// 明るさ設定を描画に適用
  /// </summary>
  void ApplyBrightness() const;

  GameSettings();
  ~GameSettings();

private:
  static std::shared_ptr<GameSettings> instance_;

  double volume_ = 0.5;       // デフォルト音量
  double brightness_ = 1.0;   // デフォルト明るさ

  static constexpr const char32* kSettingsFilePath = U"settings.json";
};
