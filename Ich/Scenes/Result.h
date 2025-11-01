#pragma once
#include "stdafx.h"
#include "Scenes/Enum.h"
#include "System/SaveData/SaveData.hpp"

class Result : public SceneManager<EnumScene, SaveData>::Scene
{
public:
  Result(const InitData& init);
  ~Result() override = default;

  void update() override;
  void draw() const override;

private:
  void DrawWordList() const;
  bool IsAnyKeyTriggered() const;

  SaveData& shared_data_;
  Texture game_clear_texture_;
  Texture game_over_texture_;
  Font title_font_;
  Font word_font_;
  Font instruction_font_;
};
