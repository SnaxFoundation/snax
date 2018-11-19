/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#pragma once

#include <snaxlib/snax.hpp>

namespace proxy {

   //@abi action
   struct set_owner {
      account_name owner;
      uint32_t     delay;

      SNAXLIB_SERIALIZE( set_owner, (owner)(delay) )
   };

   //@abi table
   struct config {
      config(){}
      constexpr static uint64_t key = N(config);
      account_name        owner = 0;
      uint32_t            delay = 0;
      uint32_t            next_id = 0;
   };

} /// namespace proxy
