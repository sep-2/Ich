#pragma once

#include "System/Task/TaskManager.h"

/// <summary>
/// タスク処理
/// </summary>
/// <remarks>
/// 各オブジェクトをTaskクラスから派生することで、
/// TaskManagerで一元管理する設計。
/// 必ずオーバーライドしてほしい関数は純粋仮想関数で用意し、
/// 各派生先クラスで専用処理を実装していく。
/// </remarks>
class Task {

public:

  /// <summary>
  /// コンストラクタ
  /// </summary>
  explicit Task();

  /// <summary>
  /// デストラクタ
  /// </summary>
  virtual ~Task();

public:

  /// <summary>
  /// タスクIDを設定
  /// </summary>
  /// <param name="task_id">設定するタスクID</param>
  void SetTaskId(TaskId task_id);

  /// <summary>
  /// タスクIDを取得する
  /// </summary>
  /// <returns>タスクID</returns>
  TaskId GetTaskId();

  /// <summary>
  /// 解放設定されているか
  /// </summary>
  /// <returns>解放設定されているならtrue</returns>
  bool IsRelease();

  /// <summary>
  /// 解放を設定する
  /// </summary>
  void SetRelease();

  /// <summary>
  /// 毎フレーム更新処理
  /// </summary>
  /// <param name="delta_time">前回実行フレームからの経過時間（秒）</param>
  virtual void Update(float delta_time);

  /// <summary>
  /// 毎フレームの描画処理
  /// </summary>
  virtual void Render();

private:

  /// <summary>
  /// タスクID
  /// </summary>
  TaskId task_id_;

  /// <summary>
  /// 解放するかの有無
  /// </summary>
  bool is_release_;

};
