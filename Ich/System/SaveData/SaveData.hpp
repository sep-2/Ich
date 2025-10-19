#pragma once
#include "stdafx.h"

/// <summary>
/// ゲームのセーブデータ
/// </summary>
struct SaveData
{
  double click_count_;

  Array<int32> item_counts_;

  /// <summary>
  /// シリアライズに対応させるためのメンバ関数を定義する
  /// </summary>
  /// <typeparam name="Archive"></typeparam>
  /// <param name="archive"></param>
  template <class Archive>
  void SIV3D_SERIALIZE(Archive& archive)
  {
    archive(click_count_, item_counts_);
  }
};
