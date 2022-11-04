// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2017-2018 The Circle Foundation & Conceal Devs
// Copyright (c) 2018-2022 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <stdexcept>
#include <algorithm>
#include <string>

#include "IWalletLegacy.h"
#include "ITokenised.h"

namespace cn {
class ISerializer;

struct UnconfirmedTransferDetails;
struct WalletLegacyTransaction;
struct WalletLegacyTransfer;

struct token_send;
struct token_transaction_data;

struct DepositInfo;
struct Deposit;
struct UnconfirmedSpentDepositDetails;

void serialize(UnconfirmedTransferDetails& utd, ISerializer& serializer);
void serialize(UnconfirmedSpentDepositDetails& details, ISerializer& serializer);
void serialize(WalletLegacyTransaction& txi, ISerializer& serializer);
void serialize(WalletLegacyTransfer& tr, ISerializer& serializer);
void serialize(token_send& tr, ISerializer& serializer);
void serialize(token_transaction_data& txi, ISerializer& serializer);
void serialize(DepositInfo& depositInfo, ISerializer& serializer);
void serialize(Deposit& deposit, ISerializer& serializer);

}
