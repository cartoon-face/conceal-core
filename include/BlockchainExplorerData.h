// Copyright (c) 2012-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2023 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <array>
#include <string>
#include <vector>

#include "CryptoTypes.h"
#include "CryptoNote.h"

#include <boost/variant.hpp>

namespace cn {

enum class TransactionRemoveReason : uint8_t 
{ 
  INCLUDED_IN_BLOCK = 0, 
  TIMEOUT = 1
};

struct TransactionOutputToKeyDetails {
  crypto::PublicKey txOutKey;
};

struct TransactionOutputMultisignatureDetails {
  std::vector<crypto::PublicKey> keys;
  uint32_t requiredSignatures;
};

struct TransactionOutputTokenDetails {
  std::vector<crypto::PublicKey> keys;
  uint32_t requiredSignatures;
};

struct TransactionOutputDetails {
  uint64_t amount;
  uint32_t globalIndex;

  boost::variant<
    TransactionOutputToKeyDetails,
    TransactionOutputMultisignatureDetails,
    TransactionOutputTokenDetails> output;
};

struct TransactionOutputReferenceDetails {
  crypto::Hash transactionHash;
  size_t number;
};

struct TransactionInputGenerateDetails {
  uint32_t height;
};

struct TransactionInputToKeyDetails {
  std::vector<uint32_t> outputIndexes;
  crypto::KeyImage keyImage;
  uint64_t mixin;
  TransactionOutputReferenceDetails output;
};

struct TransactionInputMultisignatureDetails {
  uint32_t signatures;
  TransactionOutputReferenceDetails output;
};

struct TransactionInputTokenDetails
{
  uint32_t signatures;
  TokenBase token_details;
  TransactionOutputReferenceDetails output;
};

struct TransactionInputDetails {
  uint64_t amount;

  boost::variant<
    TransactionInputGenerateDetails,
    TransactionInputToKeyDetails,
    TransactionInputMultisignatureDetails,
    TransactionInputTokenDetails> input;
};

struct TransactionExtraDetails {
  std::vector<size_t> padding;
  std::vector<crypto::PublicKey> publicKey; 
  std::vector<std::string> nonce;
  std::vector<uint8_t> raw;
};

struct TransactionDetails {
  crypto::Hash hash;
  uint64_t size;
  uint64_t fee;
  uint64_t totalInputsAmount;
  uint64_t totalOutputsAmount;
  uint64_t mixin;
  uint64_t unlockTime;
  uint64_t timestamp;
  crypto::Hash paymentId;
  bool inBlockchain;
  crypto::Hash blockHash;
  uint32_t blockHeight;
  TransactionExtraDetails extra;
  std::vector<std::vector<crypto::Signature>> signatures;
  std::vector<TransactionInputDetails> inputs;
  std::vector<TransactionOutputDetails> outputs;
};

struct BlockDetails {
  uint8_t majorVersion;
  uint8_t minorVersion;
  uint64_t timestamp;
  crypto::Hash prevBlockHash;
  uint32_t nonce;
  bool isOrphaned;
  uint32_t height;
  crypto::Hash hash;
  uint64_t difficulty;
  uint64_t reward;
  uint64_t baseReward;
  uint64_t blockSize;
  uint64_t transactionsCumulativeSize;
  uint64_t alreadyGeneratedCoins;
  uint64_t alreadyGeneratedTransactions;
  uint64_t sizeMedian;
  double penalty;
  uint64_t totalFeeAmount;
  std::vector<TransactionDetails> transactions;
};

}
