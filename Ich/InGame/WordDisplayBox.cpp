#include "stdafx.h"
#include "WordDisplayBox.h"

// 色定数の定義
const ColorF WordDisplayBox::kTextShadowColor{ 0.0, 0.0, 0.0, 0.3 };
const ColorF WordDisplayBox::kDefaultTextColor{ 0.0, 0.0, 0.0 };
const ColorF WordDisplayBox::kCompletedTextColor{ 0.8, 0.0, 0.0 };
const ColorF WordDisplayBox::kDefaultBoxColor{ 1.0, 1.0, 1.0 };
const ColorF WordDisplayBox::kCompletedBoxColor{ 1.0, 0.8, 0.8 };
const ColorF WordDisplayBox::kDefaultBorderColor{ 0.3, 0.3, 0.3 };
const ColorF WordDisplayBox::kCompletedBorderColor{ 1.0, 0.0, 0.0 };

namespace WordDisplayBoxConstants
{
  /// <summary>
  /// キューの最大サイズ
  /// </summary>
  const int kQueueSize = 7;
}

WordDisplayBox::WordDisplayBox()
  : position_x_(kDefaultPositionX)
  , position_y_(kDefaultPositionY)
  , box_size_(kDefaultBoxSize)
  , box_spacing_(kDefaultBoxSpacing)
  , previous_word_count_(0)
{
  for (int i = 0; i < WordDisplayBoxConstants::kQueueSize; i++)
  {
    CharacterEffect effect;
    word_effects_queue_.push_back(effect);
  }
}

WordDisplayBox::~WordDisplayBox()
{
}

void WordDisplayBox::Update(double deltaTime)
{
  // エフェクトの更新
  for (auto& effect : effects_)
  {
    if (effect.active)
    {
      effect.elapsed += deltaTime;
      
      // エフェクト終了判定
      if (effect.elapsed >= kEffectTotalDuration)
      {
        effect.active = false;
        effect.elapsed = 0.0;
      }
    }
  }

  for (auto& word_effect : word_effects_queue_)
  {
    if (word_effect.active)
    {
      word_effect.elapsed += deltaTime;
      // エフェクト終了判定
      if (word_effect.elapsed >= kEffectTotalDuration)
      {
        word_effect.active = false;
        word_effect.elapsed = 0.0;
      }
    }
  }
}

void WordDisplayBox::Draw(const Font& blockFont) const
{
    for (int i = word_effects_queue_.size() - 1; 0 <= i; i--) {
      // ボックスの基本位置を計算（縦方向に配置：上から下）
      const int32 box_x = position_x_;
      const int32 box_y = position_y_ + static_cast<int32>(i) * (box_size_ + box_spacing_);

      // エフェクトのスケール計算（現在位置に基づく）
      double scale = 1.0;

      //if (!word_effects_queue_.at(i).active) continue;

      if (word_effects_queue_.at(i).active) {

        // この文字のエフェクト開始タイミングを計算（下から順番に）
        const double char_delay = static_cast<double>(i) * kEffectDelayPerChar;
        const double char_elapsed = word_effects_queue_.at(i).elapsed - char_delay;

        // まだエフェクトが始まっていない場合はスキップ
        //if (char_elapsed < 0.0) continue;
        //if (char_elapsed < 0.0) break;

        if (char_elapsed < kEffectExpandDuration) {
          // 拡大フェーズ（0.0秒 → 0.5秒：1.0倍 → 2.0倍）
          const double t = char_elapsed / kEffectExpandDuration;
          scale = 1.0 + (kEffectMaxScale - 1.0) * t;
        }
        else if (char_elapsed < kEffectTotalDuration) {
          // 縮小フェーズ（0.5秒 → 1.0秒：2.0倍 → 1.0倍）
          const double t = (char_elapsed - kEffectExpandDuration) / kEffectShrinkDuration;
          scale = kEffectMaxScale - (kEffectMaxScale - 1.0) * t;
        }
      }

      // スケール適用後のボックスサイズ
      const double scaled_box_size = box_size_ * scale;
      const Vec2 box_center{ box_x + box_size_ / 2.0, box_y + box_size_ / 2.0 };

      // ボックスの背景色
      const ColorF box_color = kDefaultBoxColor;
      const ColorF border_color = kDefaultBorderColor;

      // スケール変換を適用してボックスを描画
      {
        const Transformer2D transformer{ Mat3x2::Scale(scale, box_center) };

        // ボックスを描画（角丸四角形）
        RoundRect{ box_x, box_y, box_size_, box_size_, kRoundRadius }.draw(box_color);
        RoundRect{ box_x, box_y, box_size_, box_size_, kRoundRadius }.drawFrame(kFrameThickness, border_color);
      }

      // 文字を中央に描画（影付き）- こちらもスケール適用
      {
        const Transformer2D transformer{ Mat3x2::Scale(scale, box_center) };
        const Vec2 text_center{ box_x + box_size_ / 2.0, box_y + box_size_ / 2.0 };

        // 影
        blockFont(word_effects_queue_.at(i).target_word).drawAt(text_center + kTextShadowOffset, kTextShadowColor);

        // 文字本体
        const ColorF text_color = kDefaultTextColor;
        blockFont(word_effects_queue_.at(i).target_word).drawAt(text_center, text_color);
      }

    }

  //for (size_t i = 0; i < words_.size(); i++)
  //{
  //  const String& word = words_[i];

  //  // この文字が完成した単語に含まれているかチェック
  //  const bool is_completed = IsInCompletedWord(word);

  //  // ボックスの基本位置を計算（縦方向に配置：上から下）
  //  const int32 box_x = position_x_;
  //  const int32 box_y = position_y_ + static_cast<int32>(i) * (box_size_ + box_spacing_);

  //  // エフェクトのスケール計算（現在位置に基づく）
  //  double scale = 1.0;
  //  
  //  //// 現在のインデックスがエフェクト対象かチェック
  //  //for (const auto& effect : effects_)
  //  //{
  //  //  if (!effect.active) continue;
  //  //  
  //  //  // エフェクト対象の単語を現在のキューから探す
  //  //  // 単語の各文字を下から順番に探す
  //  //  Array<size_t> target_indices;
  //  //  size_t search_start = 0;
  //  //  
  //  //  for (size_t char_idx = 0; char_idx < effect.target_word.length(); char_idx++)
  //  //  {
  //  //    const String target_char = effect.target_word.substr(char_idx, 1);
  //  //    bool found = false;
  //  //    
  //  //    // search_startから順番に探す（前から順に使う）
  //  //    for (size_t word_idx = search_start; word_idx < words_.size(); word_idx++)
  //  //    {
  //  //      if (words_[word_idx] == target_char)
  //  //      {
  //  //        target_indices.push_back(word_idx);
  //  //        search_start = word_idx + 1;
  //  //        found = true;
  //  //        break;
  //  //      }
  //  //    }
  //  //    
  //  //    if (!found)
  //  //    {
  //  //      // 単語の文字が見つからない場合は、このエフェクトは適用しない
  //  //      target_indices.clear();
  //  //      break;
  //  //    }
  //  //  }
  //  //  
  //  //  // 現在の文字がエフェクト対象かチェック
  //  //  if (target_indices.includes(i))
  //  //  {
  //  //    // 単語内でこの文字が何番目かを探す
  //  //    size_t char_position = 0;
  //  //    for (size_t j = 0; j < target_indices.size(); j++)
  //  //    {
  //  //      if (target_indices[j] == i)
  //  //      {
  //  //        char_position = j;
  //  //        break;
  //  //      }
  //  //    }
  //  //    
  //  //    // この文字のエフェクト開始タイミングを計算（下から順番に）
  //  //    const double char_delay = static_cast<double>(char_position) * kEffectDelayPerChar;
  //  //    const double char_elapsed = effect.elapsed - char_delay;
  //  //    
  //  //    // まだエフェクトが始まっていない場合はスキップ
  //  //    if (char_elapsed < 0.0) continue;
  //  //    
  //  //    if (char_elapsed < kEffectExpandDuration)
  //  //    {
  //  //      // 拡大フェーズ（0.0秒 → 0.5秒：1.0倍 → 2.0倍）
  //  //      const double t = char_elapsed / kEffectExpandDuration;
  //  //      scale = 1.0 + (kEffectMaxScale - 1.0) * t;
  //  //    }
  //  //    else if (char_elapsed < kEffectTotalDuration)
  //  //    {
  //  //      // 縮小フェーズ（0.5秒 → 1.0秒：2.0倍 → 1.0倍）
  //  //      const double t = (char_elapsed - kEffectExpandDuration) / kEffectShrinkDuration;
  //  //      scale = kEffectMaxScale - (kEffectMaxScale - 1.0) * t;
  //  //    }
  //  //    
  //  //    break;  // エフェクト見つかったので終了
  //  //  }
  //  //}

  //  // スケール適用後のボックスサイズ
  //  const double scaled_box_size = box_size_ * scale;
  //  const Vec2 box_center{ box_x + box_size_ / 2.0, box_y + box_size_ / 2.0 };

  //  // ボックスの背景色
  //  const ColorF box_color = kDefaultBoxColor;
  //  const ColorF border_color = kDefaultBorderColor;

  //  // スケール変換を適用してボックスを描画
  //  {
  //    const Transformer2D transformer{ Mat3x2::Scale(scale, box_center) };
  //    
  //    // ボックスを描画（角丸四角形）
  //    RoundRect{ box_x, box_y, box_size_, box_size_, kRoundRadius }.draw(box_color);
  //    RoundRect{ box_x, box_y, box_size_, box_size_, kRoundRadius }.drawFrame(kFrameThickness, border_color);
  //  }

  //  // 文字を中央に描画（影付き）- こちらもスケール適用
  //  {
  //    const Transformer2D transformer{ Mat3x2::Scale(scale, box_center) };
  //    const Vec2 text_center{ box_x + box_size_ / 2.0, box_y + box_size_ / 2.0 };

  //    // 影
  //    blockFont(word).drawAt(text_center + kTextShadowOffset, kTextShadowColor);
  //    
  //    // 文字本体
  //    const ColorF text_color = kDefaultTextColor;
  //    blockFont(word).drawAt(text_center, text_color);
  //  }
  //}

}

void WordDisplayBox::SetWords(const Array<String>& words)
{
  words_ = words;
  
  // エフェクト配列のサイズは動的に変更されるため、ここでは調整しない
}

void WordDisplayBox::PushCharacter(const String character)
{
  // 新しいエフェクトを追加
  CharacterEffect effect;
  effect.target_word = character;
  effect.elapsed = 0.0;
  effect.active = false;

  word_effects_queue_.push_back(effect);
  if (WordDisplayBoxConstants::kQueueSize < word_effects_queue_.size()) {
    word_effects_queue_.pop_front();
  }
}

void WordDisplayBox::SetCompletedWords(const Array<String>& completedWords)
{
  completed_words_ = completedWords;
}

void WordDisplayBox::SetPosition(int32 x, int32 y)
{
  position_x_ = x;
  position_y_ = y;
}

void WordDisplayBox::SetBoxSize(int32 size)
{
  box_size_ = size;
}

void WordDisplayBox::SetBoxSpacing(int32 spacing)
{
  box_spacing_ = spacing;
}

void WordDisplayBox::TriggerCompletionEffect(const String& completedWord)
{
  // 新しいエフェクトを追加（完成した単語の文字列を保存）
  CharacterEffect effect;
  effect.target_word = completedWord;  // 完成した単語を保存
  effect.elapsed = 0.0;
  effect.active = true;
  
  effects_.push_back(effect);

  //word_effects_queue_.push_back(effect);
  completed_word_length_ = static_cast<int32>(completedWord.length());
  for (int i = 0; i < completed_word_length_; i++) {
    word_effects_queue_.at(word_effects_queue_.size() - 1 - i).active = true;
  }
}

bool WordDisplayBox::IsInCompletedWord(const String& word) const
{
  // 空文字列の場合は完成していないと判断
  if (word.isEmpty())
  {
    return false;
  }

  for (const auto& completed_word : completed_words_)
  {
    if (completed_word.includes(word))
    {
      return true;
    }
  }
  return false;
}
