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
            uint32_t attention_rate_rating_position;
            vector<uint32_t> stat_diff;
            uint8_t tweets_ranked_in_period;
        };

        struct account_to_add {
            account_name name;
            uint64_t id;
            double attention_rate;
            uint32_t attention_rate_rating_position;
            string verification_tweet;
            string verification_salt;
            vector<uint32_t> stat_diff;
        };

        platform( account_name s )
        :contract(s), _users(s, s), _accounts(s, s), _platform_state(s, s), _pending_accounts(s, s)
         {}

        /// @abi action initialize
        void initialize(string name, account_name token_dealer, string token_symbol_str, uint8_t precision, account_name airdrop);

        /// @abi action lockupdate
        void lockupdate();

        /// @abi action nextround
        void nextround();

        /// @abi action sendpayments
        void sendpayments(account_name lower_account_name, uint64_t account_count);

        /// @abi action addpenacc
        void addpenacc(const account_name account, const uint64_t id);

        /// @abi action droppenacc
        void droppenacc(const account_name account);

        /// @abi action updatear
        void updatear(
            uint64_t id,
            double attention_rate,
            uint32_t attention_rate_rating_position,
            vector<uint32_t> stat_diff,
            uint8_t tweets_ranked_in_period,
            bool add_account_if_not_exist
        );

        /// @abi action updatearmult
        void updatearmult(vector<account_with_attention_rate>& updates, bool add_account_if_not_exist);

        /// @abi action dropaccount
        void dropaccount(account_name account, uint32_t max_account_count);

        /// @abi action addaccount
        void addaccount(
            account_name account,
            uint64_t id,
            double attention_rate,
            uint32_t attention_rate_rating_position,
            string verification_tweet,
            string verification_salt,
            vector<uint32_t> stat_diff
        );

        /// @abi action addaccounts
        void addaccounts(vector<account_to_add>& accounts_to_add);

        /// @abi action transfertou
        void transfertou(account_name from, uint64_t to, asset amount);


    private:

        /// @abi table peaccounts i64
        struct pending_rec {
            account_name account;
            uint64_t id;
            block_timestamp created;

            uint64_t primary_key() const {
                return account;
            }

            uint64_t by_id() const {
                return id;
            }

            uint64_t by_created() const {
                return created.to_time_point().time_since_epoch().count();
            }

            SNAXLIB_SERIALIZE(pending_rec, (account)(id)(created))
        };

        /// @abi table transfers i64
        struct transfer_rec {
            uint64_t id;
            asset amount;

            uint64_t primary_key() const {
                return amount.symbol.name();
            }

            SNAXLIB_SERIALIZE(transfer_rec, (id)(amount))
        };

        /// @abi table paccounts i64
        struct account {
            uint64_t id;
            account_name name;
            uint16_t last_paid_step_number;
            string verification_tweet;
            string verification_salt;
            block_timestamp created;
            vector<uint32_t> stat_diff;

            uint64_t primary_key() const {
                return id;
            }

            uint64_t by_name() const {
                return name;
            }

            uint64_t by_created() const {
                return created.to_time_point().time_since_epoch().count();
            }

            SNAXLIB_SERIALIZE(account, (id)(name)(last_paid_step_number)(verification_tweet)(verification_salt)(created)(stat_diff))
        };

        /// @abi table pusers i64
        struct user {
            uint64_t id;
            double attention_rate;
            uint32_t attention_rate_rating_position;
            uint16_t last_attention_rate_updated_step_number;
            uint8_t tweets_ranked_in_last_period;

            uint64_t primary_key() const {
                return id;
            }

            uint64_t by_attention_rate_rating_position() const {
                return attention_rate_rating_position;
            }

            SNAXLIB_SERIALIZE(user, (id)(attention_rate)(attention_rate_rating_position)(last_attention_rate_updated_step_number)(tweets_ranked_in_last_period))
        };

        /// @abi table state i64
        struct state {
            string platform_name;
            uint8_t updating;
            uint16_t step_number;
            uint64_t registered_user_count;
            uint64_t total_user_count;
            double total_attention_rate;
            account_name token_dealer;
            account_name account;
            account_name airdrop;
            asset round_supply;
            asset sent_amount;
            uint64_t round_updated_account_count;

            uint64_t primary_key() const {
                return account;
            }

            SNAXLIB_SERIALIZE(
                state, (platform_name)(updating)(step_number)(registered_user_count)(total_user_count)(total_attention_rate)(token_dealer)(account)(airdrop)(round_supply)(sent_amount)(round_updated_account_count)
            )
        };

        struct account_with_balance {
            asset    balance;
            uint64_t primary_key()const { return balance.symbol.name(); }
        };

        typedef multi_index<N(accounts), account_with_balance> _users_balances;
        typedef multi_index<N(transfers), transfer_rec> transfers_table;
        typedef multi_index<N(paccounts), account,
                indexed_by<N(name), const_mem_fun<account, uint64_t, &account::by_name>>,
                indexed_by<N(created), const_mem_fun<account, uint64_t, &account::by_created>>
        > registered_account_table;
        typedef multi_index<N(pusers), user,
                indexed_by<N(attention_rate_rating_position), const_mem_fun<user, uint64_t, &user::by_attention_rate_rating_position>>
        > usertable;
        typedef multi_index<N(peaccounts), pending_rec, indexed_by<N(created), const_mem_fun<pending_rec, uint64_t, &pending_rec::by_created>>> peacctable;
        typedef singleton<N(state), state> platform_state;

        usertable _users;
        peacctable _pending_accounts;
        platform_state _platform_state;
        state _state;
        registered_account_table _accounts;

        // Only contract itself is allowed to unlock update
        void unlock_update(asset current_amount, asset sent_amount);

        account find_account(account_name account);

        void require_initialized();

        void require_uninitialized();

        void update_state_next_round();

        void update_state_total_attention_rate_and_user_count(double additional_attention_rate, uint64_t new_accounts, uint64_t new_registered_accounts);

        asset get_balance(account_name account, symbol_type symbol_name);

        void claim_transfered(uint64_t id, account_name account);

    };

} /// namespace snax
