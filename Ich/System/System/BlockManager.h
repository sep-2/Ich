#pragma once

#include <Siv3D.hpp>
#include <utility>

class BlockManager
{
public:
  BlockManager();
  ~BlockManager();

  Array<String> GetHitWords(const Array<String>& blocks, const Array<String>& dictionary) const;
  Array<std::pair<String, String>> GetReachWords(const Array<String>& blocks, const Array<String>& dictionary) const;
};
