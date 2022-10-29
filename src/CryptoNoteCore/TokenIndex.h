// Copyright (c) 2018-2022 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

namespace cn {
class ISerializer;

class TokenIndex {
public:
  TokenIndex();
  explicit TokenIndex(uint32_t expectedHeight);

  void pushBlock(int64_t amount, uint64_t interest); 
  void popBlock(); 
  void reserve(uint32_t expectedHeight);
  size_t popBlocks(uint32_t from);

  
  int64_t tokens_at_height(uint32_t height, uint64_t token_id) const;
  int64_t full_token_amount(uint64_t token_id) const; 

  int64_t depositAmountAtHeight(uint32_t height) const;
  uint32_t size() const;

  void serialize(ISerializer& s);

private:
  struct TokenIndexEntry {
    uint32_t height;
    int64_t amount;
    uint64_t token_id;

    void serialize(ISerializer& s);
  };

  using IndexType = std::vector<TokenIndexEntry>;
  IndexType::const_iterator upperBound(uint32_t height) const;
  IndexType::const_iterator token_find(uint64_t token_id) const;
  IndexType index;
  uint32_t blockCount;
};
}
