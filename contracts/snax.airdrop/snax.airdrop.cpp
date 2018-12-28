#include "snax.airdrop.hpp"

using namespace std;

namespace snax {

    /// @abi action addplatform
    void airdrop::addplatform(const account_name platform, const asset amount_per_account) {
        require_auth(_self);
        check_platform_registered(platform);

        snax_assert(_platform_definitions.find(platform) == _platform_definitions.end(), "platform already added");

        _platform_definitions.emplace(_self, [&](auto& definition) {
            definition.platform = platform;
            definition.amount_per_account = amount_per_account;
        });
    }

    /// @abi action upplatform
    void airdrop::upplatform(const account_name platform, const asset amount_per_account) {
        require_auth(_self);
        check_platform_registered(platform);
        const auto& found = _platform_definitions.find(platform);

        snax_assert(found != _platform_definitions.end(), "platform not found");

        _platform_definitions.modify(found, _self, [&](auto& definition) {
            definition.platform = platform;
            definition.amount_per_account = amount_per_account;
        });
    }

    /// @abi action request
    void airdrop::request(const account_name platform, const account_name account) {
        require_auth(platform);
        check_platform_registered(platform);

        const auto& definition = _platform_definitions.find(platform);
        snax_assert(definition != _platform_definitions.end(), "paltform doesnt exist in airdrop configuration");

        _requested_accounts_table _requested_accounts(_self, platform);

        const asset balance = get_balance(definition->amount_per_account.symbol.name());

        if (balance >= definition->amount_per_account && _requested_accounts.find(account) == _requested_accounts.end()) {
            action(permission_level{_self, N(active)},
                   N(snax.token), N(transfer),
                   make_tuple(_self, account, definition->amount_per_account, string("airdrop payment"))
            ).send();
            _requested_accounts.emplace(_self, [&](auto& record) { record.name = account; });
        }
    }

    asset airdrop::get_balance(const uint64_t symbol_name) {
        _accounts_balances balances(N(snax.token), _self);
        const auto& airdrop_balance = balances.find(symbol_name);
        return airdrop_balance != balances.end() ? airdrop_balance->balance: asset(0, symbol_name);
    }

    void airdrop::check_platform_registered(const account_name platform) {
        _snax_global_state _state(N(snax), N(snax));
        const auto& state = _state.get();
        for (auto& platform_config: state.platforms) {
            if (platform_config.account == platform) return;
        }
        snax_assert(false, "platform doesnt exist in snax global state");
    }

}

SNAX_ABI(snax::airdrop, (addplatform)(request))
