#include "stdafx.h"

#include <Siv3D.hpp>

#include "Scenes/Enum.h"
#include "Scenes/Title.h"
#include "Scenes/InGame.h"
#include "Scenes/Result.h"
#include "System/Task/TaskManager.h"
#include "System/Renderer/Renderer.h"
#include "System/SaveData/SaveData.hpp"

namespace MainConstants
{
  // ウィンドウサイズ
  constexpr int32 kWindowWidth = 1280;
  constexpr int32 kWindowHeight = 720;

  const int kFontSize = 16;
}

// ステートの型は String
using App = SceneManager<EnumScene, SaveData>;

#if _DEBUG
namespace {
  // デバッグ用フォント（遅延初期化）
  Font* g_debug_font = nullptr;
}
#endif

/// <summary>
/// フレームレート描画用
/// </summary>
void DrawFrameRate()
{
#if _DEBUG
  if (!g_debug_font) {
    return;  // フォントが初期化されていない場合は何もしない
  }

  const int32 fps = Profiler::FPS();
  const double frameTime = Scene::DeltaTime() * 1000.0; // ミリ秒

  // 背景を描画（見やすくするため）
  RectF{ 600, 10, 180, 60 }.draw(ColorF{ 0.0, 0.0, 0.0, 0.7 });

  // FPSを描画
  (*g_debug_font)(U"FPS: {}"_fmt(fps)).draw(600, 20, Palette::White);

  // フレーム時間を描画
  (*g_debug_font)(U"Frame: {:.2f}ms"_fmt(frameTime)).draw(600, 40, Palette::White);
#endif
}

void Main()
{
  // 画面サイズを1280x720に設定
  Window::Resize(MainConstants::kWindowWidth, MainConstants::kWindowHeight);
  Window::SetTitle(U"Ich Game - 1280x720");

  // ウィンドウを閉じるユーザアクションのみを終了操作に設定する
  System::SetTerminationTriggers(UserAction::CloseButtonClicked);

#if _DEBUG
  // デバッグフォントを初期化（エンジン起動後）
  Font debug_font{ MainConstants::kFontSize };
  g_debug_font = &debug_font;
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
  manager.add<Title>(EnumScene::kTitle);
  manager.add<Game>(EnumScene::kInGame);
  manager.add<Result>(EnumScene::kResult);

  // 最初のシーンを指定
  //manager.init(EnumScene::kTitle);
  manager.init(EnumScene::kInGame);

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
  g_debug_font = nullptr;
#endif

  task_manager->Destroy();
}

