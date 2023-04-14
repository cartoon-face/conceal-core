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
    uint64_t token_id = 0;
    uint64_t token_supply = 0;
    uint64_t decimals = 0;
    uint64_t created_height = 0;
    std::string ticker = "";
    std::string token_name = "";
//

// Use for moving tokens
    uint64_t token_amount = 0;
    bool is_creation{false};
//

// for mining tokens
    uint64_t token_block_reward = 0;
    bool is_mineable{false};
//

// Serialize all data
    void serialize(cn::ISerializer& serializer) {
      serializer(token_id, "token_id");
      serializer(token_supply, "token_supply");
      serializer(decimals, "decimals");
      serializer(created_height, "created_height");
      serializer(ticker, "ticker");
      serializer(token_name, "token_name");
      serializer(token_amount, "token_amount");
      serializer(is_creation, "is_creation");
      // don't serialize mining data yet
    }
//
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
    std::string token_name;

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