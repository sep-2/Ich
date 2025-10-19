#pragma once

#include <vector>
#include <memory>

/// <summary>
/// int型をTaskIdという名前で再定義
/// </summary>
typedef unsigned int TaskId;

/// <summary>
/// Taskクラスの前方宣言
/// このheader内でTask*は扱えるようになる
/// </summary>
class Task;

/// <summary>
/// タスクマネージャー処理
/// </summary>
/// <remarks>
/// 各オブジェクトはタスクから派生してインスタンス化し、
/// タスクマネージャーではインスタンスを一元管理する
/// </remarks>
class TaskManager {
public:
  /// <summary>
  /// インスタンス所得
  /// </summary>
  /// <returns></returns>
  static TaskManager* GetInstance() {
    if (instance_ == nullptr) {
      // private ctor でも OK（メンバ関数内のため）
      instance_ = std::shared_ptr<TaskManager>(new TaskManager());
    }

    return instance_.get();
  }

  /// <summary>
  /// TaskManagerのインスタンスを削除
  /// ゲーム終了時に必ず呼ぶ
  /// </summary>
  static void Destroy() {
    if (instance_ != nullptr) {
      instance_ = nullptr;
    }
  }

public:
  /// <summary>
  /// コンストラクタ
  /// </summary>
  TaskManager();

  /// <summary>
  /// デストラクタ
  /// </summary>
  ~TaskManager();

public:
  /// <summary>
  /// タスクを追加する
  /// </summary>
  /// <param name="task">追加するタスク</param>
  /// <returns>発行したTaskId</returns>
  TaskId AddTask(std::shared_ptr<Task> task);

  /// <summary>
  /// タスクを解放する
  /// </summary>
  /// <param name="task_id">タスクID</param>
  /// <returns>成功：解放したタスク、失敗：nullptr</returns>
  std::shared_ptr<Task> ReleaseTask(TaskId task_id);

  /// <summary>
  /// タスクの毎フレーム更新処理を実行する
  /// </summary>
  /// <param name="delta_time">最後のフレームを完了するのに要した時間(秒)</param>
  void UpdateTask(float delta_time);

  /// <summary>
  /// タスクの毎フレーム更新処理を実行する
  /// </summary>
  void RenderTask();

private:
  /// <summary>
  /// TaskIDの開始番号
  /// </summary>
  static const TaskId kStartTaskId;

// メンバ変数
private:
  /// <summary>
  /// selfインスタンス
  /// </summary>
  static std::shared_ptr<TaskManager> instance_;

  /// <summary>
  /// タスクリスト
  /// </summary>
  std::vector<std::shared_ptr<Task>> task_list_;

  /// <summary>
  /// 追加タスクリスト
  /// </summary>
  std::vector<std::shared_ptr<Task>> add_task_list_;

  /// <summary>
  /// 解放タスクリスト
  /// </summary>
  std::vector<TaskId> release_task_list_;

  /// <summary>
  /// 最後に割り振ったタスクID
  /// </summary>
  int last_assigned_task_id_;
};
