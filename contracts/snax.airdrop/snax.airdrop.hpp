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
#include <snax.system/snax.system.hpp>
#include <math.h>

#include <string>

namespace snaxsystem {
    class system_contract;
}

namespace snax {
    using namespace std;

    class airdrop : public contract {
    public:
        airdrop( account_name s )
        :_platform_definitions(s, s), contract(s)
         {}

        /// @abi action addplatform
        void addplatform(account_name platform, asset amount_per_account);

        /// @abi action upplatform
        void upplatform(account_name platform, asset amount_per_account);

        /// @abi action request
        void request(account_name platform, account_name account);


    private:

        /// @abi table requested i64
        struct account_n {
            account_name name;

            uint64_t primary_key() const {
                return name;
            }

            SNAXLIB_SERIALIZE(account_n, (name))
        };

        /// @abi table platforms i64
        struct platform_def {
            account_name platform;
            asset amount_per_account;

            uint64_t primary_key() const {
                return platform;
            }

            SNAXLIB_SERIALIZE(platform_def, (platform)(amount_per_account))
        };

        struct account_with_balance {
            asset    balance;
            uint64_t primary_key()const { return balance.symbol.name(); }
        };

        typedef multi_index<N(accounts), account_with_balance> _accounts_balances;
        typedef multi_index<N(requested), account_n> _requested_accounts_table;
        typedef multi_index<N(platforms), platform_def> _platform_definitions_table;
        typedef multi_index<N(platforms), snax::platform_config> _platforms_table;

        _platform_definitions_table _platform_definitions;

        asset get_balance(uint64_t symbol_name);

        void check_platform_registered(account_name platform);

    };

} /// namespace snax
