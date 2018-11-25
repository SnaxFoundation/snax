#include "airdrop.hpp"

using namespace std;

namespace snax {

    /// @abi action addplatform
    void airdrop::addplatform(const account_name platform, const asset amount_per_account) {
        require_auth(_self);

        snax_assert(_platform_definitions.find(platform) == _platform_definitions.end(), "platform already added");

        _platform_definitions.emplace(_self, [&](auto& definition) {
            definition.platform = platform;
            definition.amount_per_account = amount_per_account;
        });
    }

    /// @abi action request
    void airdrop::request(const account_name platform, const account_name account) {
        require_auth(platform);

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

    asset airdrop::get_balance(const symbol_name symbol) {
        _accounts_balances balances(N(snax.token), _self);
        const auto platform_balance = *balances.find(symbol);
        return platform_balance.balance;
    }

}

SNAX_ABI(snax::airdrop, (addplatform)(request))
