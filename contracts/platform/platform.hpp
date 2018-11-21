/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */
#pragma once

#include <snaxlib/asset.hpp>
#include <snaxlib/snax.hpp>
#include <snaxlib/print.hpp>
#include <snaxlib/types.hpp>
#include <snaxlib/time.hpp>
#include <snaxlib/asset.hpp>
#include <snaxlib/singleton.hpp>
#include <math.h>

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
        };

        struct account_to_add {
            account_name name;
            uint64_t id;
            double attention_rate;
        };

        platform( account_name s )
        :contract(s), _accounts(s, s), _states(s, s)
         {}

        /// @abi action initialize
        void initialize(account_name token_dealer, string token_symbol_str, uint8_t precision);

        /// @abi action lockupdate
        void lockupdate();

        /// @abi action nextround
        void nextround();

        /// @abi action sendpayments
        void sendpayments(uint64_t serial, uint64_t account_count);

        /// @abi action updatear
        void updatear(uint64_t id, double attention_rate);

        /// @abi action updatearmult
        void updatearmult(vector<account_with_attention_rate> &updates);

        /// @abi action addaccount
        void addaccount(account_name account, uint64_t id, double attention_rate);

        /// @abi action addaccounts
        void addaccounts(vector<account_to_add> &accounts_to_add);


    private:

        /// @abi table paccounts i64
        struct account {
            account_name name;
            uint64_t id;
            double attention_rate;
            uint64_t serial;
            uint16_t last_updated_step_number;

            uint64_t primary_key() const {
                return id;
            }

            uint64_t by_account() const {
                return name;
            }

            uint64_t by_serial() const {
                return serial;
            }

            SNAXLIB_SERIALIZE(account, (name)(id)(attention_rate)(serial)(last_updated_step_number))
        };

        /// @abi table state i64
        struct state {
            uint16_t updating;
            uint64_t step_number;
            uint64_t account_count;
            double total_attention_rate;
            account_name token_dealer;
            account_name account;
            asset round_supply;
            uint64_t round_updated_account_count;

            uint64_t primary_key() const {
                return account;
            }

            SNAXLIB_SERIALIZE(
                state, (updating)(step_number)(account_count)(total_attention_rate)(token_dealer)(account)(round_supply)(round_updated_account_count)
            )
        };

        struct account_with_balance {
            asset    balance;
            uint64_t primary_key()const { return balance.symbol.name(); }
        };

        typedef multi_index<N(accounts), account_with_balance> _accounts_balances;
        typedef multi_index<N(paccounts), account, indexed_by<N(serial), const_mem_fun<account, uint64_t, &account::by_serial>>, indexed_by<N(name), const_mem_fun<account, uint64_t, &account::by_account>>> acctable;
        typedef multi_index<N(state), state> statetable;

        acctable _accounts;
        statetable _states;

        // Only contract itself is allowed to unlock update
        void unlock_update(asset current_amount);

        account find_account(account_name account);

        void update_state_next_round();

        void update_state_total_attention_rate_and_user_count(double additional_attention_rate, uint64_t new_accounts);

        asset get_balance();

        double convert_asset_to_double(asset value) const;

        state get_state();

        double get_token_portion(
                const double account_attention_rate,
                const double total_attention_rate
        ) const;
    };

} /// namespace snax