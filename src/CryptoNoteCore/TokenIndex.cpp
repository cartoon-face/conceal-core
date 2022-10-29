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

TokenIndex::TokenIndex(uint32_t expectedHeight) : blockCount(0) {
  index.reserve(expectedHeight + 1);
}

void TokenIndex::reserve(uint32_t expectedHeight) {
  index.reserve(expectedHeight + 1);
}

auto TokenIndex::full_token_amount(uint64_t token_id) const -> int64_t {
  auto tk = token_find(token_id);
  if ((--tk)->token_id > 0)
  {
    return index.empty() ? 0 : index.back().amount;
  }
  return 0;
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

void TokenIndex::pushBlock(int64_t amount, uint64_t interest) {
  int64_t lastAmount;
  uint64_t lastInterest;
  if (index.empty()) {
    lastAmount = 0;
    lastInterest = 0;
  } else {
    lastAmount = index.back().amount;
    //lastInterest = index.back().interest;
  }

  assert(!sumWillOverflow(amount, lastAmount));
  assert(!sumWillOverflow(interest, lastInterest));
  assert(amount + lastAmount >= 0);
  if (amount != 0) {
    index.push_back({blockCount, amount + lastAmount, interest + lastInterest});
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
  
auto TokenIndex::size() const -> uint32_t {
  return blockCount;
}

auto TokenIndex::upperBound(uint32_t height) const -> IndexType::const_iterator {
  return std::upper_bound(
      index.cbegin(), index.cend(), height,
      [] (uint32_t height, const TokenIndexEntry& left) { return height < left.height; });
}

auto TokenIndex::token_find(uint64_t token_id) const -> IndexType::const_iterator
{
  return std::upper_bound(index.cbegin(), index.cend(), token_id,
      [] (uint64_t token_id, const TokenIndexEntry& info)
      {
        return token_id < info.token_id;
      });
}

size_t TokenIndex::popBlocks(uint32_t from) {
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

auto TokenIndex::tokens_at_height(uint32_t height, uint64_t token_id) const -> int64_t {
  if (blockCount == 0)
  {
    return 0;
  }
  else
  {
    auto tk = token_find(token_id);
    if ((--tk)->token_id > 0)
    {
      auto it = upperBound(height);
      return it == index.cbegin() ? 0 : (--it)->amount;
    }
  }
  return 0;
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
