# !/bin/bash

pkill snaxnode
clisnax wallet stop
rm -rf /tmp/snaxnode_data
rm -rf ~/snax-wallet
./snaxnode.sh &

sleep 3
clisnax wallet create --file wall
clisnax wallet import --private-key 5HvtgZn4wf4vNAe3nRb9vjYfLqvasemsSQckVHxmdAeBRbdPURs
clisnax wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
clisnax wallet import --private-key 5JD9AGTuTeD5BXZwGQ5AtwBqHK21aHmYnTetHgk1B3pjj7krT8N
clisnax wallet import --private-key 5JcWXD3XkpEYbwiVK9Pd3X5bLxLkaUkkJiST3Y9iA4wFrTeyeVL
clisnax wallet import --private-key 5JLYkoKuNXGGvUtzjRnP8DqUwt7xny3YGVaDpeqFDdCJKBoBkNC
clisnax wallet import --private-key 5JRjkPFeRVGMRLaAa5gkGiC2acf8KT4NkAw1SZ5R7S1gvcCawZh

clisnax create account snax test1 SNAX74owX6TWxsbQDgYbHnwiae8ssQeCzShHE9pdSEDxo46e25yKxe SNAX74owX6TWxsbQDgYbHnwiae8ssQeCzShHE9pdSEDxo46e25yKxe
clisnax create account snax test.transf SNAX74owX6TWxsbQDgYbHnwiae8ssQeCzShHE9pdSEDxo46e25yKxe SNAX74owX6TWxsbQDgYbHnwiae8ssQeCzShHE9pdSEDxo46e25yKxe
clisnax create account snax platform SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax test2 SNAX74owX6TWxsbQDgYbHnwiae8ssQeCzShHE9pdSEDxo46e25yKxe SNAX74owX6TWxsbQDgYbHnwiae8ssQeCzShHE9pdSEDxo46e25yKxe
clisnax create account snax snax.token SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.msig SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.stake SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.ramfee SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.transf SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.ram SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.team SNAX5zwaVbLo5Jj6Uhota1c9uUNT1nuNBuv5RoGawEMYra2v9iGM3h SNAX5zwaVbLo5Jj6Uhota1c9uUNT1nuNBuv5RoGawEMYra2v9iGM3h
clisnax create account snax snax.empty SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.airdrop SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.creator SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.util SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax set contract snax ../build/contracts/snax.bios -p snax@owner
clisnax set contract snax.msig ../build/contracts/snax.msig -p snax.msig@owner
clisnax push action snax setpriv '["snax.token", 1]' -p snax@active
clisnax push action snax setpriv '["platform", 1]' -p snax@active
clisnax set contract snax.token ../build/contracts/snax.token -p snax.token@owner
clisnax push action snax.token create '["snax", "100000000000.0000 SNAX"]' -p snax.token@active
clisnax push action snax.token create '["snax", "100000000000.0000 SNIX"]' -p snax.token@active
clisnax push action snax.token issue '["test.transf", "1000.0000 SNIX", ""]' -p snax@active
clisnax push action snax.token issue '["test.transf", "1000.0000 SNAX", ""]' -p snax@active
clisnax set contract snax ../build/contracts/snax.system -p snax@owner
clisnax push action snax setparams '[{   "max_block_net_usage": 1048576,"target_block_net_usage_pct": 1000,"max_transaction_net_usage": 524288,"base_per_transaction_net_usage": 12,"net_usage_leeway": 500,"context_free_discount_net_usage_num": 20,"context_free_discount_net_usage_den": 100,"max_block_cpu_usage": 150000000,"target_block_cpu_usage_pct": 1000,"max_transaction_cpu_usage": 15000000,"min_transaction_cpu_usage": 100, "resources_market_open": 1, "top_producers_limit": 21, "enabled_contracts_by_non_privileged_users": 0,"contract_owner":1,"max_transaction_lifetime": 15000,"deferred_trx_expiration_window": 5000,"max_transaction_delay": 3888000,"max_inline_action_size": 4096,"max_inline_action_depth": 4,"max_authority_depth": 6,"max_ram_size": "68719476736", "platforms": [{"account": "platform", "weight": 1, "period": 1 }] }]' -p snax@owner
clisnax set contract platform ../build/contracts/platform -p platform@owner
