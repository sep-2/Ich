#pragma once

#include <queue>
#include <vector>
#include <utility>
#include <memory>

#include "System/Task/Task.h"
#include "System/Renderer/Priority.h"

class Renderer : public Task {
public:
  /// <summary>
  /// Rendererのインスタンスを取得する
  /// （シングルトンデザインパターン）
  /// </summary>
  /// <returns>GameInfoのインスタンス</returns>
  static Renderer* GetInstance() {

    //生成されてないなら動的生成
    if (instance_ == nullptr) {
      instance_ = std::make_shared<Renderer>();
    }

    //GameInfoのインスタンスを返す
    return instance_.get();
  }

  /// <summary>
  /// GameInfoのインスタンスを削除
  /// 
  /// ゲーム終了時に必ず呼ぶ
  /// </summary>
  static void Destroy() {
    //生成済みなら削除
    if (instance_ != nullptr) {
      //delete instance_;
      instance_ = nullptr;
    }
  }

private:


public:
  /// <summary>
/// コンストラクタ
/// </summary>
  Renderer();

  /// <summary>
  /// デストラクタ
  /// </summary>
  ~Renderer();

  /// <summary>
  /// Initialize
  /// </summary>
  void Init();

  /// <summary>
  /// Rendererリストに追加する
  /// </summary>
  /// <param name="priority">優先度、高ければ高いほど手前に画像を出す</param>
  /// <param name="image"></param>
  void Push(int priority, std::shared_ptr<Task> image);

  /// <summary>
  /// 毎フレーム更新処理
  /// </summary>
  /// <param name="delta_time">前回実行フレームからの経過時間（秒）</param>
  void Update(float delta_time) override;

  /// <summary>
  /// 毎フレームの描画処理
  /// </summary>
  void Render() override;

  /// <summary>
  /// 削除
  /// </summary>
  void Delete();

private:

  /// <summary>
  /// Rendererインスタンス
  /// （シングルトン用の静的インスタンス）
  /// </summary>
  static std::shared_ptr<Renderer> instance_;

  /// <summary>
  /// 描画リスト
  /// </summary>
  std::priority_queue<
    std::pair<int, std::shared_ptr<Task>>,
    std::vector<std::pair<int, std::shared_ptr<Task>>>,
    std::greater<std::pair<int, std::shared_ptr<Task>>>> render_list_;
};
