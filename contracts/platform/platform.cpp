#include "platform.hpp"

using namespace std;

namespace snax {

/// @abi action initialize
void platform::initialize(const string name, const account_name token_dealer,
                          const string token_symbol_str,
                          const uint8_t precision, const account_name airdrop) {
  require_auth(_self);
  require_uninitialized();

  snax_assert(name.size() > 0, "platform name can't be empty");

  const symbol_type token_symbol =
      string_to_symbol(precision, token_symbol_str.c_str());

  _state.round_supply = asset(0, token_symbol);
  _state.step_number = 1;
  _state.token_dealer = token_dealer;
  _state.total_attention_rate = 0.0;
  _state.registered_attention_rate = 0.0;
  _state.round_sent_account_count = 0;
  _state.round_updated_account_count = 0;
  _state.total_user_count = 0;
  _state.registered_user_count = 0;
  _state.updating = 0;
  _state.account = _self;
  _state.airdrop = airdrop;
  _state.platform_name = name;
  _state.sent_amount = asset(0, token_symbol);
  _state.token_symbols = {token_symbol};

  _platform_state.set(_state, _self);
}

/// @abi action lockarupdate
void platform::lockarupdate() {
  require_auth(_self);
  require_initialized();
  _state = _platform_state.get();

  snax_assert(!_state.updating, "platform is already updating");

  _state.updating = 1;
  _state.total_attention_rate = 0.0;
  _state.registered_attention_rate = 0.0;

  _platform_state.set(_state, _self);
}

/// @abi action lockupdate
void platform::lockupdate() {
  require_auth(_self);
  require_initialized();
  _state = _platform_state.get();

  snax_assert(_state.updating == 1, "platform must be in ar updating state");

  action(permission_level{_self, N(active)}, _state.token_dealer,
         N(lockplatform), make_tuple(_self))
      .send();

  _state.updating = 2;
  _platform_state.set(_state, _self);
}

/// @abi action addcreator
void platform::addcreator(const account_name name) {
  require_auth(_self);
  require_initialized();
  snax_assert(is_account(name), "account isnt registered");

  const auto found = _creators.find(name);

  snax_assert(found == _creators.end(), "creator already registered");

  _creators.emplace(_self, [&](auto &record) { record.account = name; });
}

/// @abi action rmcreator
void platform::rmcreator(const account_name name) {
  require_auth(_self);
  require_initialized();
  snax_assert(is_account(name), "account isnt registered");

  const auto found = _creators.find(name);

  snax_assert(found != _creators.end(), "creator isnt registered");

  _creators.erase(found);
}

/// @abi action nextround
void platform::nextround() {
  require_auth(_self);
  require_initialized();

  _state = _platform_state.get();

  snax_assert(
      _state.updating == 2,
      "platform must be in updating state when nextround action is called");

  action(permission_level{_self, N(active)}, _state.token_dealer,
         N(emitplatform), make_tuple(_self))
      .send();

  _state.round_sent_account_count = 0;
  _state.updating = 3;

  _platform_state.set(_state, _self);
}

/// @abi action sendpayments
void platform::sendpayments(const account_name lower_account_name,
                            uint64_t account_count) {
  require_auth(_self);
  require_initialized();
  _state = _platform_state.get();

  snax_assert(_state.updating == 3, "platform must be in updating state and "
                                    "nextround must be called before sending "
                                    "payments");

  const auto &_accounts_account_index = _accounts.get_index<N(name)>();
  auto iter = _accounts_account_index.lower_bound(
      lower_account_name ? lower_account_name : 1);

  const auto &end_iter = _accounts_account_index.cend();
  uint32_t updated_account_count = 0;

  asset current_balance = get_balance(_self, _state.round_supply.symbol);
  asset sent_amount = _state.round_supply - _state.round_supply;
  asset total_balance = _state.round_supply;

  if (_state.round_supply.amount == 0 && current_balance.amount > 0) {
    _state.round_supply = current_balance;
    total_balance = current_balance;
    _platform_state.set(_state, _self);
  }

  while (iter != end_iter && account_count--) {
    const auto &account = *iter;
    const auto &user = *_users.find(account.id);
    if (account.name && account.active) {
      updated_account_count++;
      if (user.attention_rate > 0.1 &&
          user.last_attention_rate_updated_step_number == _state.step_number) {
        snax_assert(account.last_paid_step_number < _state.step_number + 1,
                    "account already updated");

        asset token_amount;
        const int64_t portion = static_cast<int64_t>(
            _state.total_attention_rate / user.attention_rate);
        if (portion < total_balance.amount) {
          token_amount = total_balance / portion;
          if (token_amount.amount > current_balance.amount) {
            token_amount = current_balance;
          }
        } else {
          token_amount = asset(0);
        }
        if (token_amount.amount > 0) {
          current_balance -= token_amount;
          action(permission_level{_self, N(active)}, N(snax.token), N(transfer),
                 make_tuple(_self, account.name, token_amount,
                            string("payment for activity")))
              .send();
          sent_amount += token_amount;
        }
        _accounts.modify(_accounts.find(account.id), _self, [&](auto &account) {
          account.last_paid_step_number = _state.step_number;
        });
      }
    }
    iter++;
  }

  if (_state.round_sent_account_count + updated_account_count ==
      _state.registered_user_count) {
    unlock_update(current_balance, sent_amount, updated_account_count);
  } else {
    _state.round_sent_account_count += updated_account_count;
    _state.sent_amount += sent_amount;
    _platform_state.set(_state, _self);
  }
}

/// @abi action activate
void platform::activate(const uint64_t id) {
    require_auth(_self);
    require_initialized();
    set_account_active(id, true);
}

/// @abi action deactivate
void platform::deactivate(const uint64_t id) {
    require_auth(_self);
    require_initialized();
    set_account_active(id, false);
}

/// @abi action addsymbol
void platform::addsymbol(const string token_symbol_str,
                         const uint8_t precision) {
  require_auth(_self);
  require_initialized();

  const symbol_type symbol =
      string_to_symbol(precision, token_symbol_str.c_str());

  _state.token_symbols.push_back(symbol);
  _platform_state.set(_state, _self);
}

/// @abi action updatear
void platform::updatear(const uint64_t id, const double attention_rate,
                        const uint32_t attention_rate_rating_position,
                        const vector<uint32_t> stat_diff,
                        const uint8_t tweets_ranked_in_period,
                        const bool add_account_if_not_exist) {
  require_auth(_self);
  require_initialized();
  _state = _platform_state.get();

  snax_assert(
      _state.updating == 1,
      "platform must be in updating state 1 when updatear action is called");

  const auto &found = _users.find(id);

  snax_assert(found != _users.end() || add_account_if_not_exist,
              "user doesnt exist");

  double registered_attention_rate = 0;
  double total_attention_rate = 0;
  uint32_t round_updated_account_count = 0;
  uint32_t total_user_count = 0;

  if (found != _users.end()) {
    const double diff = attention_rate - found->attention_rate;

    snax_assert(attention_rate >= 0, "incorrect attention rate");

    const auto already_updated =
        found->last_attention_rate_updated_step_number == _state.step_number;

    const double attention_rate_inc =
        already_updated ? attention_rate - found->attention_rate
                        : attention_rate;

    total_attention_rate += attention_rate_inc;
    round_updated_account_count += !already_updated;

    _users.modify(found, _self, [&](auto &record) {
      record.attention_rate = attention_rate;
      record.attention_rate_rating_position = attention_rate_rating_position;
      record.last_attention_rate_updated_step_number = _state.step_number;
      record.tweets_ranked_in_last_period = tweets_ranked_in_period;
    });

    const auto &found_account = _accounts.find(id);

    if (found_account != _accounts.end()) {
      _accounts.modify(found_account, _self,
                       [&](auto &record) { record.stat_diff = stat_diff; });
      registered_attention_rate += attention_rate_inc;
    }
  } else {
    total_user_count++;
    _users.emplace(_self, [&](auto &record) {
      record.attention_rate = attention_rate;
      record.attention_rate_rating_position = attention_rate_rating_position;
      record.last_attention_rate_updated_step_number = _state.step_number;
      record.tweets_ranked_in_last_period = tweets_ranked_in_period;
      record.id = id;
    });
    addaccount(_self, 0, id, 0, string(""), stat_diff);

    total_attention_rate += attention_rate;
    round_updated_account_count++;
  }

  _state = _platform_state.get();
  _state.registered_attention_rate += registered_attention_rate;
  _state.total_attention_rate += total_attention_rate;
  _state.round_updated_account_count += round_updated_account_count;
  _state.total_user_count += total_user_count;
  _platform_state.set(_state, _self);
}

/// @abi action updatearmult
void platform::updatearmult(vector<account_with_attention_rate> &updates,
                            const bool add_account_if_not_exist) {
  require_auth(_self);
  require_initialized();
  _state = _platform_state.get();

  snax_assert(_state.updating == 1, "platform must be in updating state 1 when "
                                    "updatearmult action is called");

  double total_attention_rate = 0;
  uint32_t updated_account_count = 0;
  double registered_attention_rate = 0;
  uint32_t total_user_count = 0;

  for (auto &update : updates) {
    const auto &user = _users.find(update.id);
    snax_assert(user != _users.end() || add_account_if_not_exist,
                "user doesnt exist");
    if (user != _users.end()) {
      const double attention_rate = update.attention_rate;
      const double diff = attention_rate - user->attention_rate;
      const uint32_t attention_rate_rating_position =
          update.attention_rate_rating_position;

      snax_assert(attention_rate >= 0, "incorrect attention rate");

      const auto already_updated =
          user->last_attention_rate_updated_step_number == _state.step_number;

      const auto attention_rate_inc =
          already_updated ? attention_rate - user->attention_rate
                          : attention_rate;

      _users.modify(user, _self, [&](auto &record) {
        record.attention_rate = attention_rate;
        record.attention_rate_rating_position = attention_rate_rating_position;
        record.last_attention_rate_updated_step_number = _state.step_number;
        record.tweets_ranked_in_last_period = update.tweets_ranked_in_period;
      });

      const auto &found_account = _accounts.find(update.id);

      if (found_account != _accounts.end()) {
        _accounts.modify(found_account, _self, [&](auto &record) {
          record.stat_diff = update.stat_diff;
        });
        registered_attention_rate += attention_rate;
      }

      total_attention_rate += attention_rate_inc;
      updated_account_count += !already_updated;

    } else {
      total_user_count++;

      _users.emplace(_self, [&](auto &record) {
        record.attention_rate = update.attention_rate;
        record.attention_rate_rating_position =
            update.attention_rate_rating_position;
        record.last_attention_rate_updated_step_number = _state.step_number;
        record.tweets_ranked_in_last_period = update.tweets_ranked_in_period;
        record.id = update.id;
      });

      addaccount(_self, 0, update.id, 0, string(""), update.stat_diff);

      total_attention_rate += update.attention_rate;
      updated_account_count++;
    }
  }

  _state = _platform_state.get();
  _state.registered_attention_rate += registered_attention_rate;
  _state.total_attention_rate += total_attention_rate;
  _state.round_updated_account_count += updated_account_count;
  _state.total_user_count += total_user_count;
  _platform_state.set(_state, _self);
}

/// @abi action dropaccount
void platform::dropaccount(const account_name account,
                           uint32_t max_account_count) {
  require_auth(_self);
  require_initialized();
  _state = _platform_state.get();

  uint32_t removed_registered_accounts = 0;
  double registered_attention_rate = 0;
  while (max_account_count--) {
    const auto &_accounts_account_index = _accounts.get_index<N(name)>();
    const auto &found = _accounts_account_index.find(account);
    if (found == _accounts_account_index.end()) {
      break;
    }
    if (found->name)
      removed_registered_accounts++;
    registered_attention_rate += _users.get(found->id, "cant find user with this id").attention_rate;
    const auto found_by_id = _accounts.find(found->id);
    _accounts.erase(found_by_id);
  }

  _state = _platform_state.get();
  _state.registered_user_count -= removed_registered_accounts;
  _state.registered_attention_rate -= registered_attention_rate;
  _platform_state.set(_state, _self);
}

/// @abi action addaccount
void platform::addaccount(const account_name creator,
                          const account_name account, const uint64_t id,
                          const uint64_t verification_tweet,
                          const string verification_salt,
                          const vector<uint32_t> stat_diff) {
  require_creator_or_platform(creator);
  require_initialized();
  _state = _platform_state.get();

  const auto &found_user = _users.find(id);
  const auto &found_account = _accounts.find(id);

  double registered_attention_rate = 0;

  snax_assert(found_user == _users.end() || found_account == _accounts.end(),
              "user already exists");

  claim_transfered(id, account);

  const auto is_user_new = found_user == _users.end();

  if (is_user_new) {
    _users.emplace(_self, [&](auto &record) {
      record.id = id;
      record.last_attention_rate_updated_step_number = 0;
    });
  }

  if (account) {
    snax_assert(found_account == _accounts.end(), "account already exists");
    snax_assert(verification_tweet > 0,
                "verification tweet status id can't be empty");
    snax_assert(verification_salt.size() > 0,
                "verification salt can't be empty");

    _accounts.emplace(_self, [&](auto &record) {
      record.active = true;
      record.name = account;
      record.id = id;
      record.last_paid_step_number = 0;
      record.created = block_timestamp(snax::time_point_sec(now()));
      record.verification_tweet = verification_tweet;
      record.verification_salt = verification_salt;
      record.stat_diff = stat_diff;
    });

    if (found_user != _users.end()) {
      registered_attention_rate += found_user->attention_rate;
    }
  }

  _state = _platform_state.get();
  _state.total_user_count += is_user_new;
  _state.registered_attention_rate += registered_attention_rate;
  _state.registered_user_count += account != 0;
  _platform_state.set(_state, _self);
};

/// @abi action addaccounts
void platform::addaccounts(const account_name creator,
                           vector<account_to_add> &accounts_to_add) {
  require_creator_or_platform(creator);
  require_initialized();

  uint32_t registered_user_count = 0;

  for (auto &account_to_add : accounts_to_add) {
    addaccount(creator, account_to_add.name, account_to_add.id,
               account_to_add.verification_tweet,
               account_to_add.verification_salt, account_to_add.stat_diff);
  }
}

/// @abi action transfertou
void platform::transfertou(const account_name from, const uint64_t to,
                           const asset amount) {
  require_auth(from);
  require_initialized();

  const asset balance = get_balance(from, amount.symbol);

  snax_assert(balance >= amount, "from account doesnt have enough tokens");

  const auto &to_account = _accounts.find(to);

  if (to_account != _accounts.end() && to_account->name) {
    action(permission_level{from, N(active)}, N(snax.token), N(transfer),
           make_tuple(from, to_account->name, amount, string("social")))
        .send();
  } else {
    transfers_table _transfers(_self, amount.symbol.name());

    action(permission_level{from, N(active)}, N(snax.token), N(transfer),
           make_tuple(from, N(snax.transf), amount, string("social")))
        .send();
    const auto &found_transfer = _transfers.find(to);

    if (found_transfer != _transfers.end()) {
      _transfers.modify(found_transfer, from,
                        [&](auto &transfer) { transfer.amount += amount; });
    } else {
      _transfers.emplace(from, [&](auto &transfer) {
        transfer.amount = amount;
        transfer.id = to;
      });
    }
  }
}

asset platform::get_balance(const account_name account,
                            const symbol_type symbol) {
  require_initialized();

  _users_balances balances(N(snax.token), account);
  const auto &found_balance = balances.find(symbol.name());
  const auto result = found_balance != balances.end() ? found_balance->balance
                                                      : asset(0, symbol);
  return result;
}

void platform::claim_transfered(const uint64_t id, const account_name account) {
  require_initialized();

  if (!account)
    return;

  for (symbol_type symbol : _state.token_symbols) {
    transfers_table _transfers(_self, symbol.name());

    auto found = _transfers.find(id);

    if (found != _transfers.end()) {
      const asset amount = found->amount;

      action(permission_level{N(snax.transf), N(active)}, N(snax.token),
             N(transfer),
             make_tuple(N(snax.transf), account, amount, string("social")))
          .send();

      _transfers.erase(found++);
    }
  }
}

// Only contract itself is allowed to unlock update
void platform::unlock_update(const asset current_amount,
                             const asset last_sent_amount,
                             const uint32_t last_updated_account_count) {
  require_initialized();
  _state = _platform_state.get();

  _state.sent_amount += last_sent_amount;

  _states_history.emplace(_self, [&](auto &record) {
    record.step_number = _state.step_number;
    record.registered_user_count = _state.registered_user_count;
    record.total_user_count = _state.total_user_count;
    record.total_attention_rate = _state.total_attention_rate;
    record.registered_attention_rate = _state.registered_attention_rate;
    record.round_supply = _state.round_supply;
    record.sent_amount = _state.sent_amount;
    record.round_sent_account_count =
        _state.round_sent_account_count + last_updated_account_count;
    record.round_updated_account_count = _state.round_updated_account_count;
  });

  _state.updating = 0;
  _state.round_sent_account_count = 0;
  _state.round_updated_account_count = 0;
  _state.round_supply -= _state.round_supply;
  _state.step_number++;

  _platform_state.set(_state, _self);

  if (current_amount.amount > 0) {
    action(permission_level{_self, N(active)}, N(snax.token), N(transfer),
           make_tuple(_self, N(snax), current_amount,
                      string("rest of round supply")))
        .send();
  }
}

void platform::require_initialized() {
  snax_assert(_platform_state.exists(), "platform must be initialized");
}

void platform::require_uninitialized() {
  snax_assert(!_platform_state.exists(), "platform is already initialized");
}

void platform::require_creator_or_platform(const account_name account) {
  require_auth(account);
  snax_assert(account == _self || _creators.find(account) != _creators.end(),
              "platform or creator authority needed");
}

void platform::set_account_active(const uint64_t id, const bool active) {
    const auto found_account = _accounts.find(id);

    snax_assert(found_account != _accounts.end(), "account doesnt exist");

    _accounts.modify(found_account, 0, [&](auto& record) {
        record.active = active;
    });
}
}

SNAX_ABI(snax::platform,
         (initialize)(lockarupdate)(lockupdate)(addcreator)(rmcreator)(
             nextround)(activate)(deactivate)(addsymbol)(sendpayments)(addaccount)(dropaccount)(
             addaccounts)(updatear)(transfertou)(updatearmult))
