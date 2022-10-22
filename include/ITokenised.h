#ifndef __ITOKENISED_H__
#define __ITOKENISED_H__

// Copyright (c) 2018-2022 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

/**
 * Contains everything for tokens
 */

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

//#include "CryptoNote.h"

namespace cn
{
  struct token_tx_information
  {
    bool is_token;      // should be used to check if the transaction is a token tx
    uint64_t token_id;  // token id in the wallet
  };
  
}

#endif // __ITOKENISED_H__