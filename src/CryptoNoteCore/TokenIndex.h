// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
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
  using TokenAmount = int64_t;
  using TokenId = uint64_t;
  using TokenHeight = uint32_t;

  TokenIndex();
  explicit TokenIndex(TokenHeight expectedHeight);
  void pushBlock(TokenAmount amount, TokenId token_id); 
  void popBlock(); 
  void reserve(TokenHeight expectedHeight);
  size_t popBlocks(TokenHeight from); 
  TokenAmount tokenAmountAtHeight(TokenHeight height) const;
  TokenAmount fullTokenAmount() const; 
  TokenId fullTokenId() const;
  TokenId tokenIdAtHeight(TokenHeight height) const;
  TokenHeight size() const;
  void serialize(ISerializer& s);
private:
  struct TokenIndexEntry {
    TokenHeight height;
    TokenAmount amount;
    TokenId token_id;

    void serialize(ISerializer& s);
  };

  using IndexType = std::vector<TokenIndexEntry>;
  IndexType::const_iterator upperBound(TokenHeight height) const;
  IndexType index;
  TokenHeight blockCount;
};

}
