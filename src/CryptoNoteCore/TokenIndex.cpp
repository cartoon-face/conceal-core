// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2022 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <CryptoNoteCore/TokenIndex.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>

#include "CryptoNoteSerialization.h"
#include "Serialization/SerializationOverloads.h"

namespace cn {

TokenIndex::TokenIndex() : blockCount(0) {
}

TokenIndex::TokenIndex(TokenHeight expectedHeight) : blockCount(0) {
  index.reserve(expectedHeight + 1);
}

void TokenIndex::reserve(TokenHeight expectedHeight) {
  index.reserve(expectedHeight + 1);
}

auto TokenIndex::fullTokenAmount() const -> TokenAmount {
  return index.empty() ? 0 : index.back().amount;
}

auto TokenIndex::fullTokenId() const -> TokenId {
  return index.empty() ? 0 : index.back().token_id;
}

static inline bool sumWillOverflow(int64_t x, int64_t y) {
  if (y > 0 && x > std::numeric_limits<int64_t>::max() - y) {
    return true;
  }

  if (y < 0 && x < std::numeric_limits<int64_t>::min() - y) {
    return true;
  }
  
  return false;
}

static inline bool sumWillOverflow(uint64_t x, uint64_t y) {
  if (x > std::numeric_limits<uint64_t>::max() - y) {
    return true;
  }
 
  return false;
}

void TokenIndex::pushBlock(TokenAmount amount, TokenId token_id) {
  TokenAmount lastAmount;
  TokenId lastTokenId;
  if (index.empty()) {
    lastAmount = 0;
    lastTokenId = 0;
  } else {
    lastAmount = index.back().amount;
    lastTokenId = index.back().token_id;
  }

  //assert(!sumWillOverflow(amount, lastAmount));
  //assert(!sumWillOverflow(interest, lastInterest));
  //assert(amount + lastAmount >= 0);
  if (amount != 0) {
    index.push_back({blockCount, amount, token_id});
  }

  ++blockCount;
}

void TokenIndex::popBlock() {
  assert(blockCount > 0);
  --blockCount;
  if (!index.empty() && index.back().height == blockCount) {
    index.pop_back();
  }
}
  
auto TokenIndex::size() const -> TokenHeight {
  return blockCount;
}

auto TokenIndex::upperBound(TokenHeight height) const -> IndexType::const_iterator {
  return std::upper_bound(
      index.cbegin(), index.cend(), height,
      [] (TokenHeight height, const TokenIndexEntry& left) { return height < left.height; });
}

size_t TokenIndex::popBlocks(TokenHeight from) {
  if (from >= blockCount) {
    return 0;
  }

  IndexType::iterator it = index.begin();
  std::advance(it, std::distance(index.cbegin(), upperBound(from)));
  if (it != index.begin()) {
    --it;
    if (it->height != from) {
      ++it;
    }
  }

  index.erase(it, index.end());
  auto diff = blockCount - from;
  blockCount -= diff;
  return diff;
}

auto TokenIndex::tokenAmountAtHeight(TokenHeight height) const -> TokenAmount {
  if (blockCount == 0) {
    return 0;
  } else {
    auto it = upperBound(height);
    return it == index.cbegin() ? 0 : (--it)->amount;
  }
}

auto TokenIndex::tokenIdAtHeight(TokenHeight height) const -> TokenId {
  if (blockCount == 0) {
    return 0;
  } else {
    auto it = upperBound(height);
    return it == index.cbegin() ? 0 : (--it)->token_id;
  }
}

void TokenIndex::serialize(ISerializer& s) {
  s(blockCount, "blockCount");
  if (s.type() == ISerializer::INPUT) {
    readSequence<TokenIndexEntry>(std::back_inserter(index), "index", s);
  } else {
    writeSequence<TokenIndexEntry>(index.begin(), index.end(), "index", s);
  }
}

void TokenIndex::TokenIndexEntry::serialize(ISerializer& s) {
  s(height, "height");
  s(amount, "amount");
  s(token_id, "token_id");
}

}
