#include "stdafx.h"
#include "CollectedCharacters.h"

CollectedCharacters::CollectedCharacters()
  : max_size_(kDefaultMaxSize)
{
  // 最大文字数分の空文字列で初期化
  characters_.resize(max_size_, U"");
}

CollectedCharacters::~CollectedCharacters()
{
}

void CollectedCharacters::AddCharacter(const String& character)
{
  // 文字を追加
  characters_.push_back(character);

  // 最大文字数を超えたら先頭から削除
  while (characters_.size() > max_size_)
  {
    characters_.erase(characters_.begin());
  }
}

const Array<String>& CollectedCharacters::GetCharacters() const
{
  return characters_;
}

void CollectedCharacters::Clear()
{
  characters_.clear();
  // 最大文字数分の空文字列で初期化
  characters_.resize(max_size_, U"");
}

void CollectedCharacters::SetMaxSize(size_t maxSize)
{
  max_size_ = maxSize;

  // サイズ変更時に既存の文字配列を調整
  if (characters_.size() > max_size_)
  {
    // 最大サイズを超えている場合は先頭から削除
    characters_.erase(characters_.begin(), characters_.begin() + (characters_.size() - max_size_));
  }
  else if (characters_.size() < max_size_)
  {
    // 最大サイズより小さい場合は空文字列で埋める
    characters_.resize(max_size_, U"");
  }
}

size_t CollectedCharacters::GetMaxSize() const
{
  return max_size_;
}

size_t CollectedCharacters::GetSize() const
{
  return characters_.size();
}

bool CollectedCharacters::IsFull() const
{
  // 空文字列でないものをカウント
  size_t non_empty_count = 0;
  for (const auto& character : characters_)
  {
    if (!character.isEmpty())
    {
      ++non_empty_count;
    }
  }
  return non_empty_count >= max_size_;
}

bool CollectedCharacters::IsEmpty() const
{
  // すべて空文字列かチェック
  for (const auto& character : characters_)
  {
    if (!character.isEmpty())
    {
      return false;
    }
  }
  return true;
}

String CollectedCharacters::GetConcatenatedString() const
{
  String result;
  for (const auto& character : characters_)
  {
    result += character;
  }
  return result;
}
