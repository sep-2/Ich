#include "System/Task/TaskManager.h"
#include "System/Task/Task.h"

//タスクマネージャーのインスタンス初期化
std::shared_ptr<TaskManager> TaskManager::instance_ = nullptr;

/// <summary>
/// 割り当てるタスクIDの最初の数値
/// </summary>
const TaskId TaskManager::kStartTaskId = 100;

/// <summary>
/// コンストラクタ
/// </summary>
TaskManager::TaskManager()
  : task_list_()
  , add_task_list_()
  , release_task_list_()
  , last_assigned_task_id_(0) {
}

/// <summary>
/// デストラクタ
/// </summary>
TaskManager::~TaskManager() {
}

/// <summary>
/// タスクを追加する
/// </summary>
/// <param name="task">追加するタスク</param>
/// <returns>発行したTaskId</returns>
TaskId TaskManager::AddTask(std::shared_ptr<Task> task) {

  //追加するタスクがnullptrなら失敗
  if (!task) {
    return static_cast<TaskId>(-1);
  }

  //初の追加なら
  if (last_assigned_task_id_ == 0) {

    //開始タスクIDを設定
    last_assigned_task_id_ = kStartTaskId;
  }

  //前回割り振ったIDに+1して割り振り
  TaskId assign_id = static_cast<TaskId>(last_assigned_task_id_ + 1);

  //前回割り振ったIDを更新
  last_assigned_task_id_ = assign_id;

  //タスクにIDを設定
  task->SetTaskId(assign_id);

  //追加リストに加える
  add_task_list_.push_back(std::move(task));

  //発行したTaskIdを返す
  return assign_id;
}

/// <summary>
/// タスクを解放する
/// </summary>
/// <param name="task_id">タスクID</param>
/// <returns>成功：解放したタスク、失敗：nullptr</returns>
std::shared_ptr<Task> TaskManager::ReleaseTask(TaskId task_id) {

  std::shared_ptr<Task> find_task = nullptr;

  //タスクを見つける
  for (auto & t : task_list_) {

    //該当IDのタスクを発見した
    if (t && task_id == t->GetTaskId()) {
      find_task = t;
      break;
    }
  }

  //見つからなかったら終了
  if (!find_task) {
    return nullptr;
  }

  //タスクに解放フラグを設定する
  find_task->SetRelease();

  //削除リストに追加
  release_task_list_.push_back(task_id);

  return find_task;
}

/// <summary>
/// タスクの毎フレーム更新処理を実行する
/// </summary>
/// <param name="delta_time">最後のフレームを完了するのに要した時間 (秒) </param>
void TaskManager::UpdateTask(float delta_time) {

  //タスクの毎フレーム更新処理
  for (auto & t : task_list_) {

    //解放フラグが立っているなら実行しない
    if (!t || t->IsRelease()) {
      continue;
    }

    //タスクの更新
    t->Update(delta_time);
  }

  //追加タスクリストにあるタスクを追加
  for (auto & add_task : add_task_list_) {
    task_list_.push_back(std::move(add_task));
  }

  //追加タスクリストをクリア
  add_task_list_.clear();

  //解放タスクリストにあるタスクIDのタスクを解放
  // n^2ぐらい計算量が大きくなってしまうので、今後遅くなったら実装を変更する
  for (TaskId release_task_id : release_task_list_) {
    //タスクを見つける
    for (auto it = task_list_.begin(); it != task_list_.end(); ++it) {
      if (*it && release_task_id == (*it)->GetTaskId()) {
        task_list_.erase(it);
        break;
      }
    }
  }

  //解放タスクリストをクリア
  release_task_list_.clear();
}

/// <summary>
/// タスクの毎フレームの描画理を実行する
/// </summary>
void TaskManager::RenderTask() {

  //タスクの毎フレーム描画処理
  for (auto & t : task_list_) {

    //解放フラグが立っているなら実行しない
    if (!t || t->IsRelease()) {
      continue;
    }

    //タスクの描画
    t->Render();
  }
}
