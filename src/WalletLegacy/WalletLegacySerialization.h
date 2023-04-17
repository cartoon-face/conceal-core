// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2023 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <stdexcept>
#include <algorithm>
#include <string>

#include "IWalletLegacy.h"
#include "IToken.h"

namespace cn {
class ISerializer;

struct UnconfirmedTransferDetails;
struct WalletLegacyTransaction;
struct WalletLegacyTransfer;
struct TokenTransfer;
struct DepositInfo;
struct Deposit;
struct UnconfirmedSpentDepositDetails;
struct TokenTxInfo;
struct TokenTransactionDetails;

void serialize(UnconfirmedTransferDetails& utd, ISerializer& serializer);
void serialize(UnconfirmedSpentDepositDetails& details, ISerializer& serializer);
void serialize(WalletLegacyTransaction& txi, ISerializer& serializer);
void serialize(WalletLegacyTransfer& tr, ISerializer& serializer);
void serialize(TokenTransfer& tr, ISerializer& serializer);
void serialize(DepositInfo& depositInfo, ISerializer& serializer);
void serialize(Deposit& deposit, ISerializer& serializer);
void serialize(TokenTxInfo& token_info, ISerializer& serializer);
void serialize(TokenTransactionDetails& token_details, ISerializer& serializer);
}
