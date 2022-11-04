
#pragma once

#include <boost/optional.hpp>

#include "CryptoTypes.h"
#include "CryptoNoteCore/CryptoNoteBasic.h"

namespace cn
{
  typedef size_t TokenTxId;
  const TokenTxId WALLET_LEGACY_INVALID_TOKEN_TX_ID = std::numeric_limits<TokenTxId>::max();

  struct token_data
  {
    uint64_t token_id;
    uint64_t circulation;
    uint64_t token_txs;
  };

  struct token_send
  {
    uint64_t amount;
    std::string address;
    uint64_t token_id;
  };

  enum class token_state : uint8_t
  {
    Active,    // --> {Deleted}
    Deleted,   // --> {Active}

    Sending,   // --> {Active, Cancelled, Failed}
    Cancelled, // --> {}
    Failed     // --> {}
  };

  struct token_transaction_data
  {
    TokenTxId       firstTransferId;
    size_t          transferCount;

    uint64_t        totalAmount;
    uint64_t        fee;

    uint64_t        sentTime;
    uint64_t        unlockTime;
    uint32_t        blockHeight;
    uint64_t        timestamp;

    crypto::Hash    hash;
    boost::optional<crypto::SecretKey> secretKey = cn::NULL_SECRET_KEY;

    token_state state;
  };
  
}