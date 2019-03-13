#include "creator.hpp"

using namespace std;

namespace snax {
/// @abi action newaccount
void creator::newaccount(const account_name platform,
                         const account_name account, const uint32_t bytes,
                         const asset stake_net, const asset stake_cpu,
                         const bool transfer, const authority &owner,
                         const authority &active, const uint64_t id,
                         const uint64_t verification_tweet,
                         const string verification_salt,
                         const vector<uint32_t> stat_diff) {
  require_auth(_self);
  action(permission_level{_self, N(active)}, N(snax), N(newaccount),
         make_tuple(_self, account, owner, active))
      .send();
  action(permission_level{_self, N(active)}, N(snax), N(buyrambytes),
         make_tuple(_self, account, bytes))
      .send();
  action(permission_level{_self, N(active)}, N(snax), N(delegatebw),
         make_tuple(_self, account, stake_net, stake_cpu, transfer))
      .send();
  action(permission_level{_self, N(active)}, platform, N(addaccount),
         make_tuple(_self, account, id, verification_tweet, verification_salt,
                    stat_diff))
      .send();
}
}

SNAX_ABI(snax::creator, (newaccount))
