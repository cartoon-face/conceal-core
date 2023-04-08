// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2023 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

namespace cn {
class ISerializer;

class TokenTxIndex
{
private:
  struct TokenTxIndexEntry {
    uint64_t height;
    int64_t amount;
    uint64_t id;

    void serialize(ISerializer& s);
  };

  using IndexType = std::vector<TokenTxIndexEntry>;
  IndexType::const_iterator upperBound(uint64_t height) const;
  IndexType index;
  uint64_t blockCount;

public:
  TokenTxIndex();
  explicit TokenTxIndex(uint64_t expectedHeight);

  void pushBlock(int64_t amount, uint64_t id); 
  void popBlock(); 
  void reserve(uint64_t expectedHeight);

  uint64_t known_token_ids() const;

  size_t popBlocks(uint64_t from); 
  int64_t depositAmountAtHeight(uint64_t height) const;
  int64_t fullDepositAmount() const; 
  uint64_t depositInterestAtHeight(uint64_t height) const;
  uint64_t fullInterestAmount() const; 
  uint64_t size() const;
  void serialize(ISerializer& s);
};

class DepositIndex {
public:
  using DepositAmount = int64_t;
  using DepositInterest = uint64_t;
  using DepositHeight = uint32_t;
  DepositIndex();
  explicit DepositIndex(DepositHeight expectedHeight);
  void pushBlock(DepositAmount amount, DepositInterest interest); 
  void popBlock(); 
  void reserve(DepositHeight expectedHeight);
  size_t popBlocks(DepositHeight from); 
  DepositAmount depositAmountAtHeight(DepositHeight height) const;
  DepositAmount fullDepositAmount() const; 
  DepositInterest depositInterestAtHeight(DepositHeight height) const;
  DepositInterest fullInterestAmount() const; 
  DepositHeight size() const;
  void serialize(ISerializer& s);

private:
  struct DepositIndexEntry {
    DepositHeight height;
    DepositAmount amount;
    DepositInterest interest;

    void serialize(ISerializer& s);
  };

  using IndexType = std::vector<DepositIndexEntry>;
  IndexType::const_iterator upperBound(DepositHeight height) const;
  IndexType index;
  DepositHeight blockCount;
};
}
