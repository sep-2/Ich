#include "stdafx.h"

#include <Siv3D.hpp>

#include "Scenes/Enum.h"
#include "Scenes/Title.h"
#include "Scenes/InGame.h"
#include "System/Task/TaskManager.h"
#include "System/Renderer/Renderer.h"
#include "System/SaveData/SaveData.hpp"

// ステートの型は String
using App = SceneManager<EnumScene, SaveData>;

#if _DEBUG
namespace {
  // デバッグ用フォント（遅延初期化）
  Font* g_debugFont = nullptr;
}
#endif

/// <summary>
/// フレームレート描画用
/// </summary>
void DrawFrameRate()
{
#if _DEBUG
  if (!g_debugFont) {
    return;  // フォントが初期化されていない場合は何もしない
  }

  const int32 fps = Profiler::FPS();
  const double frameTime = Scene::DeltaTime() * 1000.0; // ミリ秒

  // 背景を描画（見やすくするため）
  RectF{ 600, 10, 180, 60 }.draw(ColorF{ 0.0, 0.0, 0.0, 0.7 });

  // FPSを描画
  (*g_debugFont)(U"FPS: {}"_fmt(fps)).draw(600, 20, Palette::White);

  // フレーム時間を描画
  (*g_debugFont)(U"Frame: {:.2f}ms"_fmt(frameTime)).draw(600, 40, Palette::White);
#endif
}

void Main()
{
  PRINT << U"Hello, Siv3D!" << 123 << 45.67;

  // 画面サイズを1280x720に設定
  Window::Resize(1280, 720);
  Window::SetTitle(U"Ich Game - 1280x720");

  // ウィンドウを閉じるユーザアクションのみを終了操作に設定する
  System::SetTerminationTriggers(UserAction::CloseButtonClicked);

#if _DEBUG
  // デバッグフォントを初期化（エンジン起動後）
  Font debugFont{ 16 };
  g_debugFont = &debugFont;
#endif

  FontAsset::Register(U"TitleFont", FontMethod::MSDF, 48, Typeface::Bold);

  // タスクマネージャーでタスクを管理する
  TaskManager* task_manager = TaskManager::GetInstance();

  // Renderer を shared_ptr でラップして AddTask
  std::shared_ptr<Renderer> renderer(Renderer::GetInstance(), [](Renderer*) {});
  task_manager->AddTask(renderer);

  // シーンマネージャーを作成
  App manager;

  // 各シーンを登録する
  manager.add<Game>(EnumScene::kInGame);
  manager.add<Title>(EnumScene::kTitle);

  while (System::Update()) {
    task_manager->UpdateTask(static_cast<float>(Scene::DeltaTime()));

    // 現在のシーンを実行する
    // シーンに実装した .update() と .draw() が実行される
    if (not manager.update()) {
      break;
    }

    task_manager->RenderTask();

    // フレームレートを描画（デバッグモードのみ）
    DrawFrameRate();
  }

#if _DEBUG
  // クリーンアップ
  g_debugFont = nullptr;
#endif

  task_manager->Destroy();
}

