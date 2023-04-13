// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2023 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "TransactionUtils.h"

#include <unordered_set>

#include "crypto/crypto.h"
#include "CryptoNoteCore/Account.h"
#include "CryptoNoteFormatUtils.h"
#include "TransactionExtra.h"
#include "IToken.h"

using namespace crypto;

namespace cn {

bool checkInputsKeyimagesDiff(const cn::TransactionPrefix& tx) {
  std::unordered_set<crypto::KeyImage> ki;
  for (const auto& in : tx.inputs) {
    if (in.type() == typeid(KeyInput)) {
      if (!ki.insert(boost::get<KeyInput>(in).keyImage).second)
        return false;
    }
  }
  return true;
}

// TransactionInput helper functions

size_t getRequiredSignaturesCount(const TransactionInput& in) {
  if (in.type() == typeid(KeyInput)) {
    return boost::get<KeyInput>(in).outputIndexes.size();
  }
  if (in.type() == typeid(MultisignatureInput)) {
    return boost::get<MultisignatureInput>(in).signatureCount;
  }
  if (in.type() == typeid(TokenInput)) {
    return boost::get<TokenInput>(in).signatureCount;
  }
  return 0;
}

uint64_t getTransactionInputAmount(const TransactionInput& in) {
  if (in.type() == typeid(KeyInput)) {
    return boost::get<KeyInput>(in).amount;
  }
  if (in.type() == typeid(MultisignatureInput)) {
    // TODO calculate interest
    return boost::get<MultisignatureInput>(in).amount;
  }
  if (in.type() == typeid(TokenInput)) {
    return boost::get<TokenInput>(in).amount;
  }
  return 0;
}

transaction_types::InputType getTransactionInputType(const TransactionInput& in) {
  if (in.type() == typeid(KeyInput)) {
    return transaction_types::InputType::Key;
  }
  if (in.type() == typeid(MultisignatureInput)) {
    return transaction_types::InputType::Multisignature;
  }
  if (in.type() == typeid(TokenInput)) {
    return transaction_types::InputType::Token;
  }
  if (in.type() == typeid(BaseInput)) {
    return transaction_types::InputType::Generating;
  }
  return transaction_types::InputType::Invalid;
}

const TransactionInput& getInputChecked(const cn::TransactionPrefix& transaction, size_t index) {
  if (transaction.inputs.size() <= index) {
    throw std::runtime_error("Transaction input index out of range");
  }
  return transaction.inputs[index];
}

const TransactionInput& getInputChecked(const cn::TransactionPrefix& transaction, size_t index, transaction_types::InputType type) {
  const auto& input = getInputChecked(transaction, index);
  if (getTransactionInputType(input) != type) {
    throw std::runtime_error("Unexpected transaction input type");
  }
  return input;
}

const TransactionInput& getInputChecked(const cn::TransactionPrefix& transaction, size_t index, transaction_types::InputType type, TokenSummary& token_details) {
  const auto& input = getInputChecked(transaction, index);
  if (getTransactionInputType(input) != type) {
    throw std::runtime_error("Unexpected transaction input type");
  }
  return input;
}

// TransactionOutput helper functions

transaction_types::OutputType getTransactionOutputType(const TransactionOutputTarget& out) {
  if (out.type() == typeid(KeyOutput)) {
    return transaction_types::OutputType::Key;
  }
  if (out.type() == typeid(MultisignatureOutput)) {
    return transaction_types::OutputType::Multisignature;
  }
  if (out.type() == typeid(TokenOutput)) {
    return transaction_types::OutputType::Token;
  }
  return transaction_types::OutputType::Invalid;
}

const TransactionOutput& getOutputChecked(const cn::TransactionPrefix& transaction, size_t index) {
  if (transaction.outputs.size() <= index) {
    throw std::runtime_error("Transaction output index out of range");
  }
  return transaction.outputs[index];
}

const TransactionOutput& getOutputChecked(const cn::TransactionPrefix& transaction, size_t index, transaction_types::OutputType type) {
  const auto& output = getOutputChecked(transaction, index);
  if (getTransactionOutputType(output.target) != type) {
    throw std::runtime_error("Unexpected transaction output target type");
  }
  return output;
}

const TransactionOutput& getOutputChecked(const cn::TransactionPrefix& transaction, size_t index, transaction_types::OutputType type, TokenSummary& token_details) {
  const auto& output = getOutputChecked(transaction, index);
  if (getTransactionOutputType(output.target) != type) {
    throw std::runtime_error("Unexpected transaction output target type");
  }
  return output;
}

bool isOutToKey(const crypto::PublicKey& spendPublicKey, const crypto::PublicKey& outKey, const crypto::KeyDerivation& derivation, size_t keyIndex) {
  crypto::PublicKey pk;
  derive_public_key(derivation, keyIndex, spendPublicKey, pk);
  return pk == outKey;
}

bool findOutputsToAccount(const cn::TransactionPrefix& transaction, const AccountPublicAddress& addr,
                          const SecretKey& viewSecretKey, std::vector<uint32_t>& out, uint64_t& amount) {
  AccountKeys keys;
  keys.address = addr;
  // only view secret key is used, spend key is not needed
  keys.viewSecretKey = viewSecretKey;

  crypto::PublicKey txPubKey = getTransactionPublicKeyFromExtra(transaction.extra);

  amount = 0;
  size_t keyIndex = 0;
  uint32_t outputIndex = 0;

  crypto::KeyDerivation derivation;
  generate_key_derivation(txPubKey, keys.viewSecretKey, derivation);

  for (const TransactionOutput& o : transaction.outputs) {
    assert(o.target.type() == typeid(KeyOutput) || o.target.type() == typeid(MultisignatureOutput) || o.target.type() == typeid(TokenOutput));
    if (o.target.type() == typeid(KeyOutput)) {
      if (is_out_to_acc(keys, boost::get<KeyOutput>(o.target), derivation, keyIndex)) {
        out.push_back(outputIndex);
        amount += o.amount;
      }
      ++keyIndex;
    } else if (o.target.type() == typeid(MultisignatureOutput)) {
      const auto& target = boost::get<MultisignatureOutput>(o.target);
      for (const auto& key : target.keys) {
        if (isOutToKey(keys.address.spendPublicKey, key, derivation, static_cast<size_t>(outputIndex))) {
          out.push_back(outputIndex);
        }
        ++keyIndex;
      }
    } else if (o.target.type() == typeid(TokenOutput)) {
      const auto& target = boost::get<TokenOutput>(o.target);
      for (const auto& key : target.keys) {
        if (isOutToKey(keys.address.spendPublicKey, key, derivation, static_cast<size_t>(outputIndex))) {
          out.push_back(outputIndex);
        }
        ++keyIndex;
      }
    }
    ++outputIndex;
  }

  return true;
}

}
