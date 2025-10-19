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

void Main()
{
  PRINT << U"Hello, Siv3D!" << 123 << 45.67;

  // ウィンドウを閉じるユーザアクションのみを終了操作に設定する
  System::SetTerminationTriggers(UserAction::CloseButtonClicked);

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

	while (System::Update())
	{
    task_manager->UpdateTask(static_cast<float>(Scene::DeltaTime()));
    task_manager->RenderTask();

		// 現在のシーンを実行する
		// シーンに実装した .update() と .draw() が実行される
		if (not manager.update())
		{
			break;
		}
	}

  task_manager->Destroy();
}
