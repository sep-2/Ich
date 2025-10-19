#include "stdafx.h"
#include "MenuSoundManager.h"

std::shared_ptr<MenuSoundManager> MenuSoundManager::instance_ = nullptr;

MenuSoundManager* MenuSoundManager::GetInstance() {
  if (instance_ == nullptr) {
    instance_ = std::make_shared<MenuSoundManager>();
  }
  return instance_.get();
}

MenuSoundManager::MenuSoundManager()
  // ホバー音: 高めの音
  : hover_sound_(GMInstrument::Celesta, PianoKey::C6, 0.1s, 0.05s, 0.3)
  // クリック音: 決定音
  , click_sound_(GMInstrument::SynthDrum, PianoKey::C5, 0.15s, 0.1s, 0.5)
  // スライダー音: 短い音
  , slider_sound_(GMInstrument::Vibraphone, PianoKey::E5, 0.05s, 0.05s, 0.2)
{
}

MenuSoundManager::~MenuSoundManager() {
}

void MenuSoundManager::PlaySe(MenuSeKind kind) {
  switch (kind) {
  case MenuSeKind::kHover:
    hover_sound_.playOneShot(0.3);
    break;
  case MenuSeKind::kClick:
    click_sound_.playOneShot(0.5);
    break;
  case MenuSeKind::kSlider:
    slider_sound_.playOneShot(0.2);
    break;
  }
}
