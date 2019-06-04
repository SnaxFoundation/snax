/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#pragma once

#include <math.h>
#include <snaxlib/asset.hpp>
#include <snaxlib/crypto.h>
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

class steem : public contract {
public:
  struct account_with_attention_rate {
    uint64_t id;
    string account_name;
    double attention_rate;
    uint32_t attention_rate_rating_position;
    vector<uint32_t> stat_diff;
    uint8_t posts_ranked_in_period;
  };

  struct account_to_add {
    account_name name;
    uint64_t id;
    string account_name;
    uint64_t verification_post;
    string verification_salt;
    vector<uint32_t> stat_diff;
  };

  steem(account_name s)
      : contract(s), _users(s, s), _accounts(s, s), _platform_state(s, s),
        _creators(s, s), _states_history(s, s), _bounty_state(s, s),
        _bounty_articles(s, s) {}

  /// @abi action addarticle
  void addarticle(const uint64_t author, const string permlink,
                  const string title, const block_timestamp created);

  /// @abi action rmarticle
  void rmarticle(const string permlink);

  /// @abi action paybounty
  void paybounty(const account_name payer, const string permlink,
                 const asset amount);

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

  /// @abi action resetupdate
  void resetupdate(const uint8_t updating_state);

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

  /// @abi action updatear
  void updatear(uint64_t id, string account_name, double attention_rate,
                uint32_t attention_rate_rating_position,
                vector<uint32_t> stat_diff, uint8_t posts_ranked_in_period,
                bool add_account_if_not_exist);

  /// @abi action updatearmult
  void updatearmult(vector<account_with_attention_rate> &updates,
                    bool add_account_if_not_exist);

  /// @abi action dropuser
  void dropuser(uint64_t id);

  /// @abi action bindaccount
  void bindaccount(account_name account, string salt);

  /// @abi action dropaccount
  void dropaccount(account_name initiator, uint64_t id);

  /// @abi action addaccount
  void addaccount(account_name creator, account_name account, uint64_t id,
                  string account_name, uint64_t verification_post,
                  string verification_salt, vector<uint32_t> stat_diff);

  /// @abi action addaccounts
  void addaccounts(account_name creator,
                   vector<account_to_add> &accounts_to_add);

  /// @abi action transfersoca
  void transfersoca(account_name from, string to, asset quantity, string memo);

  /// @abi action transfersoc
  void transfersoc(account_name from, uint64_t to, asset quantity, string memo);

private:
  /// @abi table barticles i64
  struct article {
    uint64_t seq;
    uint64_t author;
    string permlink;
    string title;
    block_timestamp created;
    asset paid;

    uint64_t primary_key() const { return seq; }

    uint64_t by_author() const { return author; }

    uint64_t by_created() const {
      return created.to_time_point().time_since_epoch().count();
    }

    key256 by_permlink() const {
      checksum256 hash;
      sha256(permlink.c_str(), permlink.size() * sizeof(char), &hash);
      return checksum256_to_sha256(hash);
    }

    SNAXLIB_SERIALIZE(article, (seq)(author)(permlink)(title)(created)(paid))
  };

  /// @abi table bounty i64
  struct bounty_state {
    asset total_paid;
    block_timestamp last_update;

    uint64_t primary_key() const {
      return last_update.to_time_point().time_since_epoch().count();
    }

    SNAXLIB_SERIALIZE(bounty_state, (total_paid)(last_update))
  };

  /// @abi table bindings i64
  struct binding {
    account_name account;
    string salt;

    uint64_t primary_key() const { return account; }

    SNAXLIB_SERIALIZE(binding, (account)(salt))
  };

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
    uint64_t verification_post;
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
                      (id)(name)(last_paid_step_number)(verification_post)(
                          verification_salt)(created)(stat_diff)(active))
  };

  /// @abi table pusers i64
  struct user {
    uint64_t id;
    double attention_rate;
    uint32_t attention_rate_rating_position;
    uint16_t last_attention_rate_updated_step_number;
    uint8_t posts_ranked_in_last_period;
    string account_name;

    uint64_t primary_key() const { return id; }

    key256 by_account_name() const {
      checksum256 hash;
      sha256(account_name.c_str(), account_name.size() * sizeof(char), &hash);
      return checksum256_to_sha256(hash);
    }

    uint64_t by_attention_rate_rating_position() const {
      return static_cast<uint64_t>(0xFFFFFFFF) *
                 static_cast<uint64_t>(
                     last_attention_rate_updated_step_number) +
             static_cast<uint64_t>(attention_rate_rating_position);
    }

    SNAXLIB_SERIALIZE(user,
                      (id)(attention_rate)(attention_rate_rating_position)(
                          last_attention_rate_updated_step_number)(
                          posts_ranked_in_last_period)(account_name))
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
                               &user::by_attention_rate_rating_position>>,
      indexed_by<N(account_name),
                 const_mem_fun<user, key256, &user::by_account_name>>>
      usertable;
  typedef multi_index<N(usercreators), creator_rec> creators_table;
  typedef multi_index<N(states), state_step> platform_states_history;
  typedef multi_index<N(bindings), binding> _account_bindings;
  typedef multi_index<
      N(barticles), article,
      indexed_by<N(author),
                 const_mem_fun<article, uint64_t, &article::by_author>>,
      indexed_by<N(permlink),
                 const_mem_fun<article, key256, &article::by_permlink>>,
      indexed_by<N(created),
                 const_mem_fun<article, uint64_t, &article::by_created>>>
      bounty_articles_table;
  typedef singleton<N(state), state> platform_state;
  typedef singleton<N(bounty), bounty_state> bounty_state_table;

  usertable _users;
  platform_state _platform_state;
  bounty_state_table _bounty_state;
  bounty_state _bounty;
  state _state;
  registered_account_table _accounts;
  creators_table _creators;
  bounty_articles_table _bounty_articles;
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

  static key256 checksum256_to_sha256(const checksum256 &hash) {
    const uint128_t *p128 = reinterpret_cast<const uint128_t *>(&hash);
    key256 k;
    k.data()[0] = p128[0];
    k.data()[1] = p128[1];
    return k;
  }
};

} /// namespace snax
