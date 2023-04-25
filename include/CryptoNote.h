// Copyright (c) 2012-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2023 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <vector>
#include <boost/variant.hpp>
#include <../src/Common/Optional.hpp>
#include "CryptoTypes.h"

namespace cn {

struct TokenBase
{
  uint64_t token_id = 0;
  uint64_t token_amount = 0;
  uint8_t decimals = 0;
  std::string ticker = "";
  std::string token_name = "";
};

struct BaseInput {
  uint32_t blockIndex;
};

struct KeyInput {
  uint64_t amount;
  std::vector<uint32_t> outputIndexes;
  crypto::KeyImage keyImage;
};

struct MultisignatureInput {
  uint64_t amount;
  uint8_t signatureCount;
  uint32_t outputIndex;
  uint32_t term;
};

struct TokenInput
{
  uint64_t amount;
  uint32_t outputIndex;
  uint8_t signatureCount;
  TokenBase token_details;
};

struct KeyOutput {
  crypto::PublicKey key;
};

struct MultisignatureOutput {
  std::vector<crypto::PublicKey> keys;
  uint8_t requiredSignatureCount;
  uint32_t term;
};

struct TokenOutput
{
  std::vector<crypto::PublicKey> keys;
  uint8_t requiredSignatureCount;
  TokenBase token_details;
};

typedef boost::variant<BaseInput, KeyInput, MultisignatureInput, TokenInput> TransactionInput;

typedef boost::variant<KeyOutput, MultisignatureOutput, TokenOutput> TransactionOutputTarget;

struct TransactionOutput {
  uint64_t amount;
  TransactionOutputTarget target;
};

using TransactionInputs = std::vector<TransactionInput>;

/*
The TransactionPrefix structure contains all the necessary information
to determine the transaction hash and create the signature,
except for the signatures themselves.
Separating the transaction into two structures allows the signature
to be calculated over the TransactionPrefix structure only, without
including the signatures themselves.
This is important for security reasons, as it helps prevent the signatures
from being tampered with or invalidated.
*/
struct TransactionPrefix {
  uint8_t version;
  uint64_t unlockTime;
  TransactionInputs inputs;
  std::vector<TransactionOutput> outputs;
  std::vector<uint8_t> extra;
  optional<TokenBase> token_details;
};

struct Transaction : public TransactionPrefix {
  std::vector<std::vector<crypto::Signature>> signatures;
};

struct BlockHeader {
  uint8_t majorVersion;
  uint8_t minorVersion;
  uint32_t nonce;
  uint64_t timestamp;
  crypto::Hash previousBlockHash;
};

struct Block : public BlockHeader {
  Transaction baseTransaction;
  std::vector<crypto::Hash> transactionHashes;
};

struct AccountPublicAddress {
  crypto::PublicKey spendPublicKey;
  crypto::PublicKey viewPublicKey;
};

struct AccountKeys {
  AccountPublicAddress address;
  crypto::SecretKey spendSecretKey;
  crypto::SecretKey viewSecretKey;
};

struct KeyPair {
  crypto::PublicKey publicKey;
  crypto::SecretKey secretKey;
};

using BinaryArray = std::vector<uint8_t>;

}
