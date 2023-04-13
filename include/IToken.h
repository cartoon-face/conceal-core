// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2023 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include <vector>
#include "CryptoNote.h"
#include "../src/Serialization/ISerializer.h"

namespace cn
{
  class TokenSummary
  {
  public:
// Store for information
    uint64_t token_id;
    uint64_t token_supply;
    uint64_t decimals;
    uint64_t created_height;
    std::string ticker;
//

// Use for moving tokens
    uint64_t token_amount;
    bool is_creation;
//

// Serialize all data
    void serialize(cn::ISerializer& serializer) {
      serializer(token_id, "token_id");
      serializer(token_supply, "token_supply");
      serializer(decimals, "decimals");
      serializer(created_height, "created_height");
      serializer(ticker, "ticker");
      serializer(token_amount, "token_amount");
      serializer(is_creation, "is_creation");
    }
  };

  struct TokenTransactionDetails
  {
    size_t transaction_id;

    uint64_t ccx_amount; // usually a fee
    uint64_t height_sent;
    uint64_t token_amount;
    uint64_t token_id;
    uint64_t decimals;
    bool     is_creation;
    std::string ticker;

    uint32_t outputInTransaction;
    crypto::Hash transactionHash;
    std::string address;
  };

  struct TokenTransfer
  {
    // standard with every tx
    int64_t amount;
    std::string address;

    // token details
    TokenSummary token_details;
  };

}