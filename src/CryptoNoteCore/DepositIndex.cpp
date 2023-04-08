// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2023 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <CryptoNoteCore/DepositIndex.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>

#include "CryptoNoteSerialization.h"
#include "Serialization/SerializationOverloads.h"

namespace cn {

TokenTxIndex::TokenTxIndex() : blockCount(0) {
}

TokenTxIndex::TokenTxIndex(uint64_t expectedHeight) : blockCount(0) {
  index.reserve(expectedHeight + 1);
}

void TokenTxIndex::reserve(uint64_t expectedHeight) {
  index.reserve(expectedHeight + 1);
}

void TokenTxIndex::pushBlock(int64_t amount, uint64_t id) {
  int64_t lastAmount;
  //uint64_t lastId;

  if (index.empty()) {
    lastAmount = 0;
    //lastId = 0;
  } else {
    lastAmount = index.back().amount;
    //lastId = index.back().id;
  }

  //assert(!sumWillOverflow(amount, lastAmount));

  assert(amount >= 0);
  if (amount != 0) {
    index.push_back({blockCount, amount + lastAmount, id});
  }

  ++blockCount;
}

void TokenTxIndex::popBlock() {
  assert(blockCount > 0);
  --blockCount;
  if (!index.empty() && index.back().height == blockCount) {
    index.pop_back();
  }
}
  
auto TokenTxIndex::size() const -> uint64_t {
  return blockCount;
}

auto TokenTxIndex::upperBound(uint64_t height) const -> IndexType::const_iterator {
  return std::upper_bound(
      index.cbegin(), index.cend(), height,
      [] (uint64_t height, const TokenTxIndexEntry& left) { return height < left.height; });
}

size_t TokenTxIndex::popBlocks(uint64_t from) {
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

uint64_t TokenTxIndex::known_token_ids() const {
  return index.empty() ? 0 : index.back().id;
}

// TODO more...

void TokenTxIndex::serialize(ISerializer& s) {
  s(blockCount, "blockCount");
  if (s.type() == ISerializer::INPUT) {
    readSequence<TokenTxIndexEntry>(std::back_inserter(index), "index", s);
  } else {
    writeSequence<TokenTxIndexEntry>(index.begin(), index.end(), "index", s);
  }
}

void TokenTxIndex::TokenTxIndexEntry::serialize(ISerializer& s) {
  s(height, "height");
  s(amount, "amount");
  s(id, "id");
}

// --------------------------------------------------------------------------------------------------------

DepositIndex::DepositIndex() : blockCount(0) {
}

DepositIndex::DepositIndex(DepositHeight expectedHeight) : blockCount(0) {
  index.reserve(expectedHeight + 1);
}

void DepositIndex::reserve(DepositHeight expectedHeight) {
  index.reserve(expectedHeight + 1);
}

auto DepositIndex::fullDepositAmount() const -> DepositAmount {
  return index.empty() ? 0 : index.back().amount;
}

auto DepositIndex::fullInterestAmount() const -> DepositInterest {
  return index.empty() ? 0 : index.back().interest;
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

void DepositIndex::pushBlock(DepositAmount amount, DepositInterest interest) {
  DepositAmount lastAmount;
  DepositInterest lastInterest;
  if (index.empty()) {
    lastAmount = 0;
    lastInterest = 0;
  } else {
    lastAmount = index.back().amount;
    lastInterest = index.back().interest;
  }

  assert(!sumWillOverflow(amount, lastAmount));
  assert(!sumWillOverflow(interest, lastInterest));
  assert(amount + lastAmount >= 0);
  if (amount != 0) {
    index.push_back({blockCount, amount + lastAmount, interest + lastInterest});
  }

  ++blockCount;
}

void DepositIndex::popBlock() {
  assert(blockCount > 0);
  --blockCount;
  if (!index.empty() && index.back().height == blockCount) {
    index.pop_back();
  }
}
  
auto DepositIndex::size() const -> DepositHeight {
  return blockCount;
}

auto DepositIndex::upperBound(DepositHeight height) const -> IndexType::const_iterator {
  return std::upper_bound(
      index.cbegin(), index.cend(), height,
      [] (DepositHeight height, const DepositIndexEntry& left) { return height < left.height; });
}

size_t DepositIndex::popBlocks(DepositHeight from) {
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

auto DepositIndex::depositAmountAtHeight(DepositHeight height) const -> DepositAmount {
  if (blockCount == 0) {
    return 0;
  } else {
    auto it = upperBound(height);
    return it == index.cbegin() ? 0 : (--it)->amount;
  }
}

auto DepositIndex::depositInterestAtHeight(DepositHeight height) const -> DepositInterest {
  if (blockCount == 0) {
    return 0;
  } else {
    auto it = upperBound(height);
    return it == index.cbegin() ? 0 : (--it)->interest;
  }
}

void DepositIndex::serialize(ISerializer& s) {
  s(blockCount, "blockCount");
  if (s.type() == ISerializer::INPUT) {
    readSequence<DepositIndexEntry>(std::back_inserter(index), "index", s);
  } else {
    writeSequence<DepositIndexEntry>(index.begin(), index.end(), "index", s);
  }
}

void DepositIndex::DepositIndexEntry::serialize(ISerializer& s) {
  s(height, "height");
  s(amount, "amount");
  s(interest, "interest");
}

}
