#include "stdafx.h"
#include "WordDisplayBox.h"

// 定数の定義
const ColorF WordDisplayBox::kTextShadowColor{ 0.0, 0.0, 0.0, 0.3 };
const ColorF WordDisplayBox::kDefaultTextColor{ 0.0, 0.0, 0.0 };
const ColorF WordDisplayBox::kCompletedTextColor{ 0.8, 0.0, 0.0 };
const ColorF WordDisplayBox::kDefaultBoxColor{ 1.0, 1.0, 1.0 };
const ColorF WordDisplayBox::kCompletedBoxColor{ 1.0, 0.8, 0.8 };
const ColorF WordDisplayBox::kDefaultBorderColor{ 0.3, 0.3, 0.3 };
const ColorF WordDisplayBox::kCompletedBorderColor{ 1.0, 0.0, 0.0 };

WordDisplayBox::WordDisplayBox()
  : position_x_(50)
  , position_y_(20)
  , box_size_(50)
  , box_spacing_(5)
{
}

WordDisplayBox::~WordDisplayBox()
{
}

void WordDisplayBox::Update(double deltaTime)
{
  // 現在は更新処理なし（将来的にアニメーションなどを追加可能）
}

void WordDisplayBox::Draw(const Font& blockFont) const
{
  for (int i = 0; i < words_.size(); i++) {
    const String& word = words_[i];

    // この文字が完成した単語に含まれているかチェック
    const bool is_completed = IsInCompletedWord(word);

    // ボックスの位置を計算（縦方向に配置：上から下）
    const int32 box_x = position_x_;
    const int32 box_y = position_y_ + i * (box_size_ + box_spacing_);

    // ボックスの背景色（完成した単語に含まれる場合は明るい赤、それ以外は白）
    const ColorF box_color = is_completed ? kCompletedBoxColor : kDefaultBoxColor;
    const ColorF border_color = is_completed ? kCompletedBorderColor : kDefaultBorderColor;

    // ボックスを描画（角丸四角形）
    RoundRect{ box_x, box_y, box_size_, box_size_, kRoundRadius }.draw(box_color);
    RoundRect{ box_x, box_y, box_size_, box_size_, kRoundRadius }.drawFrame(kFrameThickness, border_color);

    // 文字を中央に描画（影付き）
    const Vec2 text_center{ box_x + box_size_ / 2.0, box_y + box_size_ / 2.0 };

    // 影
    blockFont(word).drawAt(text_center + kTextShadowOffset, kTextShadowColor);

    // 文字本体（完成した単語に含まれる場合は赤、それ以外は黒）
    const ColorF text_color = is_completed ? kCompletedTextColor : kDefaultTextColor;
    blockFont(word).drawAt(text_center, text_color);
  }
}

void WordDisplayBox::SetWords(const Array<String>& words)
{
  words_ = words;
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

bool WordDisplayBox::IsInCompletedWord(const String& word) const
{
  for (const auto& completed_word : completed_words_) {
    if (completed_word.includes(word)) {
      return true;
    }
  }
  return false;
}
