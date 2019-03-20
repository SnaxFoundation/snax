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

class platform : public contract {
public:
  struct account_with_attention_rate {
    uint64_t id;
    double attention_rate;
    uint32_t attention_rate_rating_position;
    vector<uint32_t> stat_diff;
    uint8_t tweets_ranked_in_period;
  };

  struct account_to_add {
    account_name name;
    uint64_t id;
    uint64_t verification_tweet;
    string verification_salt;
    vector<uint32_t> stat_diff;
  };

  platform(account_name s)
      : contract(s), _users(s, s), _accounts(s, s), _platform_state(s, s),
        _creators(s, s), _states_history(s, s) {}

  /// @abi action addcreator
  void addcreator(const account_name name);

  /// @abi action rmcreator
  void rmcreator(const account_name name);

  /// @abi action initialize
  void initialize(string name, account_name token_dealer,
                  string token_symbol_str, uint8_t precision,
                  account_name airdrop);

  /// @abi action lockarupdate
  void lockarupdate();

  /// @abi action lockupdate
  void lockupdate();

  /// @abi action nextround
  void nextround();

  /// @abi action addsymbol
  void addsymbol(const string token_symbol_str, const uint8_t precision);

  /// @abi action sendpayments
  void sendpayments(account_name lower_account_name, uint64_t account_count);

  /// @abi action activate
  void activate(uint64_t id);

  /// @abi action deactivate
  void deactivate(uint64_t id);

  /// @abi action addpenacc
  void addpenacc(const account_name account, const uint64_t id);

  /// @abi action droppenacc
  void droppenacc(const account_name account);

  /// @abi action updatear
  void updatear(uint64_t id, double attention_rate,
                uint32_t attention_rate_rating_position,
                vector<uint32_t> stat_diff, uint8_t tweets_ranked_in_period,
                bool add_account_if_not_exist);

  /// @abi action updatearmult
  void updatearmult(vector<account_with_attention_rate> &updates,
                    bool add_account_if_not_exist);

  /// @abi action dropuser
  void dropuser(uint64_t id);

  /// @abi action dropaccount
  void dropaccount(uint64_t id);

  /// @abi action addaccount
  void addaccount(const account_name creator, account_name account, uint64_t id,
                  uint64_t verification_tweet, string verification_salt,
                  vector<uint32_t> stat_diff);

  /// @abi action addaccounts
  void addaccounts(const account_name creator,
                   vector<account_to_add> &accounts_to_add);

  /// @abi action transfertou
  void transfertou(account_name from, uint64_t to, asset amount);

private:
  /// @abi table usercreators i64
  struct creator_rec {
    account_name account;

    uint64_t primary_key() const { return account; }

    SNAXLIB_SERIALIZE(creator_rec, (account))
  };

  /// @abi table transfers i64
  struct transfer_rec {
    uint64_t id;
    asset amount;

    uint64_t primary_key() const { return id; }

    SNAXLIB_SERIALIZE(transfer_rec, (id)(amount))
  };

  /// @abi table paccounts i64
  struct account {
    uint64_t id;
    account_name name;
    uint16_t last_paid_step_number;
    uint64_t verification_tweet;
    string verification_salt;
    block_timestamp created;
    vector<uint32_t> stat_diff;
    bool active;

    uint64_t primary_key() const { return id; }

    uint64_t by_name() const { return name; }

    uint64_t by_created() const {
      return created.to_time_point().time_since_epoch().count();
    }

    SNAXLIB_SERIALIZE(account,
                      (id)(name)(last_paid_step_number)(verification_tweet)(
                          verification_salt)(created)(stat_diff)(active))
  };

  /// @abi table pusers i64
  struct user {
    uint64_t id;
    double attention_rate;
    uint32_t attention_rate_rating_position;
    uint16_t last_attention_rate_updated_step_number;
    uint8_t tweets_ranked_in_last_period;

    uint64_t primary_key() const { return id; }

    uint64_t by_attention_rate_rating_position() const {
      return static_cast<uint64_t>(0xFFFFFFFF) *
                 static_cast<uint64_t>(
                     last_attention_rate_updated_step_number) +
             static_cast<uint64_t>(attention_rate_rating_position);
    }

    SNAXLIB_SERIALIZE(user,
                      (id)(attention_rate)(attention_rate_rating_position)(
                          last_attention_rate_updated_step_number)(
                          tweets_ranked_in_last_period))
  };

  /// @abi table state i64
  struct state {
    string platform_name;
    uint8_t updating;
    uint16_t step_number;
    uint64_t registered_user_count;
    uint64_t total_user_count;
    double total_attention_rate;
    double registered_attention_rate;
    account_name token_dealer;
    account_name account;
    account_name airdrop;
    asset round_supply;
    asset sent_amount;
    uint64_t round_sent_account_count;
    uint64_t round_updated_account_count;
    vector<uint64_t> token_symbols;

    uint64_t primary_key() const { return account; }

    SNAXLIB_SERIALIZE(
        state, (platform_name)(updating)(step_number)(registered_user_count)(
                   total_user_count)(total_attention_rate)(
                   registered_attention_rate)(token_dealer)(account)(airdrop)(
                   round_supply)(sent_amount)(round_sent_account_count)(
                   round_updated_account_count)(token_symbols))
  };

  // @abi table states i64
  struct state_step {
    uint16_t step_number;
    uint64_t registered_user_count;
    uint64_t total_user_count;
    double total_attention_rate;
    double registered_attention_rate;
    asset round_supply;
    asset sent_amount;
    uint64_t round_sent_account_count;
    uint64_t round_updated_account_count;

    uint64_t primary_key() const { return step_number; }

    SNAXLIB_SERIALIZE(
        state_step,
        (step_number)(registered_user_count)(total_user_count)(
            total_attention_rate)(registered_attention_rate)(round_supply)(
            sent_amount)(round_sent_account_count)(round_updated_account_count))
  };

  struct account_with_balance {
    asset balance;
    uint64_t primary_key() const { return balance.symbol.name(); }
  };

  typedef multi_index<N(accounts), account_with_balance> _users_balances;
  typedef multi_index<N(transfers), transfer_rec> transfers_table;
  typedef multi_index<
      N(paccounts), account,
      indexed_by<N(name), const_mem_fun<account, uint64_t, &account::by_name>>,
      indexed_by<N(created),
                 const_mem_fun<account, uint64_t, &account::by_created>>>
      registered_account_table;
  typedef multi_index<
      N(pusers), user,
      indexed_by<N(attention_rate_rating_position),
                 const_mem_fun<user, uint64_t,
                               &user::by_attention_rate_rating_position>>>
      usertable;
  typedef multi_index<N(usercreators), creator_rec> creators_table;
  typedef multi_index<N(states), state_step> platform_states_history;
  typedef singleton<N(state), state> platform_state;

  usertable _users;
  platform_state _platform_state;
  state _state;
  registered_account_table _accounts;
  creators_table _creators;
  platform_states_history _states_history;

  // Only contract itself is allowed to unlock update
  void unlock_update(asset current_amount, asset sent_amount,
                     uint32_t last_updated_account_count);

  account find_account(account_name account);

  void update_state_next_round();

  asset get_balance(account_name account, symbol_type symbol_name);

  void claim_transfered(uint64_t id, account_name account);

  void require_creator_or_platform(account_name account);

  void require_initialized();

  void require_uninitialized();

  void set_account_active(uint64_t id, bool active);
};

} /// namespace snax
