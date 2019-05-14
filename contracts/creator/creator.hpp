/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#pragma once

#include <math.h>
#include <snaxlib/asset.hpp>
#include <snaxlib/print.hpp>
#include <snaxlib/singleton.hpp>
#include <snaxlib/snax.hpp>
#include <snaxlib/time.hpp>
#include <snaxlib/types.hpp>

#include <string>

namespace snaxsystem {
class system_contract;
}

namespace snax {
using namespace std;

class creator : public contract {
public:
  creator(account_name s) : contract(s) {}

  struct permission_level_weight {
    permission_level permission;
    weight_type weight;

    // explicit serialization macro is not necessary, used here only to improve
    // compilation time
    SNAXLIB_SERIALIZE(permission_level_weight, (permission)(weight))
  };

  struct key_weight {
    public_key key;
    weight_type weight;

    // explicit serialization macro is not necessary, used here only to improve
    // compilation time
    SNAXLIB_SERIALIZE(key_weight, (key)(weight))
  };

  struct wait_weight {
    uint32_t wait_sec;
    weight_type weight;

    SNAXLIB_SERIALIZE(wait_weight, (wait_sec)(weight))
  };

  struct authority {
    uint32_t threshold;
    vector<key_weight> keys;
    vector<permission_level_weight> accounts;
    vector<wait_weight> waits;

    SNAXLIB_SERIALIZE(authority, (threshold)(keys)(accounts)(waits))
  };

  /// @abi action newaccount
  void newaccount(const account_name platform, const account_name account,
                  const uint32_t bytes, const asset stake_net,
                  const asset stake_cpu, const bool transfer,
                  const authority &owner, const authority &active,
                  const uint64_t id, const uint64_t verification_post,
                  const string verification_salt,
                  const vector<uint32_t> stat_diff);

  /// @abi action newaccname
  void newaccname(const account_name platform, const account_name account,
                  const uint32_t bytes, const asset stake_net,
                  const asset stake_cpu, const bool transfer,
                  const authority &owner, const authority &active,
                  const uint64_t id, const string account_name,
                  const uint64_t verification_post,
                  const string verification_salt,
                  const vector<uint32_t> stat_diff);
};
}
