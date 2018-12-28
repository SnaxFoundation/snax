#include "platform.hpp"

using namespace std;

namespace snax {

    /// @abi action initialize
    void platform::initialize(const string name, const account_name token_dealer, const string token_symbol_str, const uint8_t precision, const account_name airdrop) {
        require_auth2(_self, N(owner));
        require_uninitialized();

        snax_assert(name.size() > 0, "platform name can't be empty");

        const auto token_symbol = string_to_symbol(precision, token_symbol_str.c_str());

        _state.round_supply = asset(0, token_symbol);
        _state.step_number = 0;
        _state.token_dealer = token_dealer;
        _state.total_attention_rate = 0.0;
        _state.round_updated_account_count = 0;
        _state.total_account_count = 0;
        _state.registered_user_count = 0;
        _state.updating = 0;
        _state.account = _self;
        _state.airdrop = airdrop;
        _state.platform_name = name;
        _state.sent_amount = asset(0, token_symbol);

        _platform_state.set(_state, _self);
    }

    /// @abi action lockupdate
    void platform::lockupdate() {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();

        _state.updating = 1;
        _platform_state.set(_state, _self);
    }

    /// @abi action nextround
    void platform::nextround() {
        require_auth2(_self, N(owner));
        require_initialized();
        _state = _platform_state.get();
        snax_assert(_state.updating == 1, "platform must be in updating state when nextround action is called");

        action(permission_level{_self, N(owner)}, _state.token_dealer, N(emitplatform), make_tuple(_self)).send();

        _state.round_updated_account_count = 0;
        _state.updating = 2;

        _platform_state.set(_state, _self);
    }

    /// @abi action sendpayments
    void platform::sendpayments(const account_name lower_account_name, uint64_t account_count) {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();

        snax_assert(_state.updating == 2, "platform must be in updating state and nextround must be called before sending payments");

        const auto account_account_index = _accounts.get_index<N(name)>();
        auto iter = account_account_index.lower_bound(lower_account_name ? lower_account_name: 1);

        const auto& end_iter = account_account_index.cend();
        uint32_t updated_account_count = 0;

        asset current_balance = get_balance(_self);
        asset sent_amount = _state.round_supply - _state.round_supply;
        asset total_balance = _state.round_supply;

        if (
            _state.round_supply.amount == 0 && current_balance.amount > 0
        ) {
            _state.round_supply = current_balance;
            total_balance = current_balance;
            _platform_state.set(_state, _self);
        }

        while (iter != end_iter && account_count--) {
            const auto& account = *_accounts.find(iter->id);
            if (account.name) {
                updated_account_count++;
                if (account.attention_rate > 0.1) {
                    snax_assert(account.last_paid_step_number < _state.step_number + 1, "account already updated");
                    asset token_amount;
                    const int64_t portion = static_cast<int64_t>(_state.total_attention_rate / account.attention_rate);
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
                        action(permission_level{_self, N(active)},
                               N(snax.token), N(transfer),
                               make_tuple(_self, account.name, token_amount, string("payment for activity"))
                        ).send();
                        sent_amount += token_amount;
                    }
                    _accounts.modify(
                        account, _self, [&](auto& account) {
                            account.last_paid_step_number = _state.step_number;
                        }
                    );
                }
            }
            iter++;
        }

        if (_state.round_updated_account_count + updated_account_count == _state.registered_user_count) {
            unlock_update(current_balance, sent_amount);
        } else {
            _state.round_updated_account_count += updated_account_count;
            _state.sent_amount += sent_amount;
            _platform_state.set(_state, _self);
        }
    }

    /// @abi action addpenacc
    void platform::addpenacc(const account_name account, const uint64_t id) {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();

        const auto& found = _pending_accounts.find(account);

        snax_assert(found == _pending_accounts.end(), "account already in pending queue");

        _pending_accounts.emplace(_self, [&](auto& record) {
            record.account = account;
            record.id = id;
            record.created = block_timestamp(snax::time_point_sec(now()));
        });
    }

    /// @abi action droppenacc
    void platform::droppenacc(const account_name account) {
        require_auth(_self);
        require_initialized();
        const auto& found_acc = _pending_accounts.find(account);

        snax_assert(found_acc != _pending_accounts.end(), "pending account not found");

        _pending_accounts.erase(found_acc);
    }

    /// @abi action updatear
    void platform::updatear(const uint64_t id, const double attention_rate, const uint32_t attention_rate_rating_position, const bool add_account_if_not_exist) {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();

        snax_assert(!_state.updating,
                     "platform mustn't be in updating state when updatear action is called");

        const auto &found = _accounts.find(id);

        snax_assert(found != _accounts.end() || add_account_if_not_exist, "user doesnt exist");

        if (found != _accounts.end()) {
            const double diff = attention_rate - found->attention_rate;

            snax_assert(diff >= 0 || abs(diff) <= abs(found->attention_rate), "incorrect attention rate");

            update_state_total_attention_rate_and_user_count(diff, 0, 0);

            _accounts.modify(
                    found, _self, [&](auto &record) {
                        record.attention_rate = attention_rate;
                        record.attention_rate_rating_position = attention_rate_rating_position;
                        record.last_attention_rate_updated_step_number = _state.step_number;
                    }
            );
        } else {
            addaccount(0, id, attention_rate, attention_rate_rating_position);
        }
    }

    /// @abi action updatearmult
    void platform::updatearmult(vector <account_with_attention_rate>& updates, const bool add_account_if_not_exist) {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();

        snax_assert(!_state.updating,
                     "platform mustn't be in updating state when updatearmult action is called");

        double total_attention_rate_diff = 0;

        for (auto& update: updates) {
            const auto& account = _accounts.find(update.id);
            snax_assert(account != _accounts.end() || add_account_if_not_exist, "user doesnt exist");
            if (account != _accounts.end()) {
                const double attention_rate = update.attention_rate;
                const double diff = attention_rate - account->attention_rate;
                const uint32_t attention_rate_rating_position = update.attention_rate_rating_position;

                snax_assert(diff >= 0 || abs(diff) <= abs(account->attention_rate), "incorrect attention rate");

                _accounts.modify(
                        account, _self, [&](auto &record) {
                            record.attention_rate = attention_rate;
                            record.attention_rate_rating_position = attention_rate_rating_position;
                            record.last_attention_rate_updated_step_number = _state.step_number;
                        }
                );

                total_attention_rate_diff += diff;
            } else {
                addaccount(0, update.id, update.attention_rate, update.attention_rate_rating_position);
            }
        }

        update_state_total_attention_rate_and_user_count(total_attention_rate_diff, 0, 0);
    }

    /// @abi action dropaccount
    void platform::dropaccount(const account_name account, uint32_t max_account_count) {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();
        snax_assert(
                !_state.updating,
                "platform must not be in updating state when dropaccount action is called"
        );

        uint32_t removed_registered_accounts = 0;
        uint32_t removed_accounts = 0;
        while (max_account_count--) {
            const auto account_index = _accounts.get_index<N(name)>();
            const auto& found = account_index.find(account);
            if (found == account_index.end()) {
                break;
            }
            if (found->name) removed_registered_accounts++;
            removed_accounts++;
            _accounts.erase(_accounts.find(found->id));
        }

        _state.total_account_count -= removed_accounts;
        _state.registered_user_count -= removed_registered_accounts;
        _platform_state.set(_state, _self);
    }

    /// @abi action addaccount
    void platform::addaccount(const account_name account, const uint64_t id, const double attention_rate, const uint32_t attention_rate_rating_position) {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();

        snax_assert(
                !_state.updating,
                "platform must not be in updating state when addaccount action is called"
        );

        snax_assert(attention_rate >= 0, "attention rate must be greater than zero or equal to zero");
        const auto& found = _accounts.find(id);
        snax_assert(found == _accounts.end() || !found->name, "user already exists");

        const auto& pending = _pending_accounts.find(account);

        if (pending != _pending_accounts.end()) {
            _pending_accounts.erase(pending);
        }

        claim_transfered(id, account);

        if (found == _accounts.end()) {
            _accounts.emplace(
                    _self, [&](auto &record) {
                        record.attention_rate = attention_rate;
                        record.id = id;
                        record.name = account;
                        record.last_attention_rate_updated_step_number = _state.step_number;
                        record.attention_rate_rating_position = attention_rate_rating_position;
                    }
            );
        } else {
            _accounts.modify(
                    found, _self, [&](auto &record) {
                        const double diff = attention_rate - record.attention_rate;

                        snax_assert(diff >= 0 || abs(diff) <= abs(record.attention_rate), "incorrect attention rate");

                        record.attention_rate = attention_rate;
                        record.name = account;
                        record.last_attention_rate_updated_step_number = _state.step_number;
                        record.attention_rate_rating_position = attention_rate_rating_position;
                    }
            );
        }

        if (_state.airdrop && account) {
            action(permission_level{_self, N(active)}, _state.airdrop, N(request), make_tuple(_self, account)).send();
        }

        update_state_total_attention_rate_and_user_count(attention_rate, 1, account != 0);
    };

    /// @abi action addaccounts
    void platform::addaccounts(vector<account_to_add> &accounts_to_add) {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();

        snax_assert(
                !_state.updating,
                "platform must not be in updating state when addaccount action is called"
        );

        double accumulated_attention_rate = 0;
        uint32_t index = 0;
        uint32_t registered_accounts = 0;

        for (auto& account_to_add: accounts_to_add) {
            const auto& found_account = _accounts.find(account_to_add.id);
            snax_assert(_accounts.find(account_to_add.id) == _accounts.end() || !found_account->name, "user already exists");
            snax_assert(account_to_add.attention_rate >= 0, "attention rate must be greater than zero or equal to zero");
            accumulated_attention_rate += account_to_add.attention_rate;

            if (account_to_add.name) {
                registered_accounts++;
            }

            const auto& pending = _pending_accounts.find(account_to_add.name);

            if (pending != _pending_accounts.end()) {
                _pending_accounts.erase(pending);
            }

            claim_transfered(account_to_add.id, account_to_add.name);

            if (found_account != _accounts.end()) {
                _accounts.modify(
                        found_account, _self, [&](auto &record) {
                            record.attention_rate = account_to_add.attention_rate;
                            record.id = account_to_add.id;
                            record.name = account_to_add.name;
                            record.attention_rate_rating_position = account_to_add.attention_rate_rating_position;
                            record.last_attention_rate_updated_step_number = _state.step_number;
                        }
                );
            } else {
                _accounts.emplace(
                        _self, [&](auto &record) {
                            record.attention_rate = account_to_add.attention_rate;
                            record.id = account_to_add.id;
                            record.name = account_to_add.name;
                            record.attention_rate_rating_position = account_to_add.attention_rate_rating_position;
                            record.last_attention_rate_updated_step_number = _state.step_number;
                        }
                );
            }
            index++;
        }

        update_state_total_attention_rate_and_user_count(accumulated_attention_rate, accounts_to_add.size(), registered_accounts);
    }

    /// @abi action trasnfertou
    void platform::transfertou(const account_name from, const uint64_t to, const asset amount) {
        require_auth(from);
        require_initialized();

        const asset balance = get_balance(from);

        snax_assert(balance >= amount, "from account doesnt have enough tokens");

        const auto& to_account = _accounts.find(to);

        if (to_account != _accounts.end() && to_account->name) {
            action(permission_level{from, N(active)}, N(snax.token), N(transfer), make_tuple(from, to_account->name, amount, string("social transaction"))).send();
        } else {
            action(permission_level{from, N(active)}, N(snax.token), N(transfer), make_tuple(from, N(snax.transf), amount, string("social transaction"))).send();
            const auto& found_transfer = _transfers.find(to);

            if (found_transfer != _transfers.end()) {
                _transfers.modify(found_transfer, _self, [&](auto& transfer) {
                    transfer.amount += amount;
                });
            } else {
                _transfers.emplace(_self, [&](auto& transfer) {
                    transfer.amount = amount;
                    transfer.id = to;
                });
            }
        }

    }

    asset platform::get_balance(const account_name account) {
        require_initialized();
        _state = _platform_state.get();

        _accounts_balances balances(N(snax.token), account);
        const auto& platform_balance = balances.find(_state.round_supply.symbol.name());
        const auto result = platform_balance != balances.end() ? platform_balance->balance: asset(0);
        return result;
    }

    void platform::update_state_total_attention_rate_and_user_count(const double additional_attention_rate, const uint64_t new_accounts, const uint64_t new_registered_accounts) {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();

        _state.total_attention_rate += additional_attention_rate;
        _state.total_account_count += new_accounts;
        _state.registered_user_count += new_registered_accounts;
        _platform_state.set(_state, _self);
    }

    void platform::claim_transfered(const uint64_t id, const account_name account) {
        require_auth(_self);
        require_initialized();

        if (!account) return;

        const auto& found = _transfers.find(id);

        if (found != _transfers.end()) {
            action(permission_level{N(snax.transf), N(active)}, N(snax.token), N(transfer), make_tuple(N(snax.transf), account, found->amount, string("social transaction"))).send();
            _transfers.erase(found);
        }
    }

    // Only contract itself is allowed to unlock update
    void platform::unlock_update(const asset current_amount, const asset last_sent_amount) {
        require_auth(_self);
        require_initialized();
        _state = _platform_state.get();

        _state.updating = 0;
        _state.round_updated_account_count = 0;
        _state.round_supply -= _state.round_supply;
        _state.sent_amount += last_sent_amount;
        _state.step_number += 1;

        _platform_state.set(_state, _self);

        if (current_amount.amount > 0) {
            action(permission_level{_self, N(active)}, N(snax.token), N(transfer), make_tuple(_self, N(snax), current_amount, string("rest of money"))).send();
        }
    }

    platform::account platform::find_account(const account_name account) {
        const auto account_index = _accounts.get_index<N(name)>();
        const auto& found = account_index.find(account);
        snax_assert(found != account_index.end(), "user doesn't exist in platform");

        return *found;
    }

    void platform::require_initialized() {
        snax_assert(_platform_state.exists(), "platform must be initialized");
    }

    void platform::require_uninitialized() {
        snax_assert(!_platform_state.exists(), "platform is already initialized");
    }
}

SNAX_ABI(snax::platform, (initialize)(lockupdate)(nextround)(addpenacc)(droppenacc)(sendpayments)(addaccount)(addaccounts)(updatear)(transfertou)
(updatearmult))
