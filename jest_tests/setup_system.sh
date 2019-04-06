# !/bin/bash

pkill snaxnode
clisnax wallet stop
rm -rf /tmp/snaxnode_data
rm -rf ~/snax-wallet
./snaxnode.sh &

sleep 4
clisnax wallet create --file wall
clisnax wallet import --private-key 5HvtgZn4wf4vNAe3nRb9vjYfLqvasemsSQckVHxmdAeBRbdPURs
clisnax wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
clisnax wallet import --private-key 5JD9AGTuTeD5BXZwGQ5AtwBqHK21aHmYnTetHgk1B3pjj7krT8N
clisnax wallet import --private-key 5JcWXD3XkpEYbwiVK9Pd3X5bLxLkaUkkJiST3Y9iA4wFrTeyeVL
clisnax wallet import --private-key 5JLYkoKuNXGGvUtzjRnP8DqUwt7xny3YGVaDpeqFDdCJKBoBkNC

clisnax create account snax snax.token SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.msig SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.stake SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.ramfee SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.ram SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.team SNAX5zwaVbLo5Jj6Uhota1c9uUNT1nuNBuv5RoGawEMYra2v9iGM3h SNAX5zwaVbLo5Jj6Uhota1c9uUNT1nuNBuv5RoGawEMYra2v9iGM3h
clisnax create account snax snax.airdrop SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.creator SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.transf SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax test.transf SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
clisnax create account snax snax.bpay SNAX82vokU8bz112PQueGtpG6qkW2tXXrAGPjsVjsNEkZRXaGH4xTe SNAX82vokU8bz112PQueGtpG6qkW2tXXrAGPjsVjsNEkZRXaGH4xTe
clisnax create account snax snax.vpay SNAX82vokU8bz112PQueGtpG6qkW2tXXrAGPjsVjsNEkZRXaGH4xTe SNAX82vokU8bz112PQueGtpG6qkW2tXXrAGPjsVjsNEkZRXaGH4xTe

clisnax set contract snax ../build/contracts/snax.bios -p snax@owner
clisnax set contract snax.msig ../build/contracts/snax.msig -p snax.msig@owner
clisnax push action snax setpriv '["snax.token", 1]' -p snax@active
clisnax push action snax setpriv '["snax.creator", 1]' -p snax@active
clisnax set contract snax.token ../build/contracts/snax.token -p snax.token@owner
clisnax push action snax.token create '["snax", "1000000000000.0000 SNAX"]' -p snax.token@active
clisnax set contract snax ../build/contracts/test_snax.system -x 1000 -p snax@owner

clisnax system newaccount snax.creator testacc1 SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV -p snax.creator@active --stake-cpu="15000.0000 SNAX" --stake-net="10000.0000 SNAX" --buy-ram-kbytes=1000
clisnax system newaccount snax.creator testacc2 SNAX8mo3cUJW1Yy1GGxQfexWGN7QPUB2rXccQP7brrpgJXGjiw6gKR  -p snax.creator@active --stake-cpu="15000.0000 SNAX" --stake-net="10000.0000 SNAX" --buy-ram-kbytes=1000
clisnax system newaccount snax.creator platform1 SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV -p snax.creator@active --stake-cpu="15000.0000 SNAX" --stake-net="10000.0000 SNAX" --buy-ram-kbytes=1000
clisnax system newaccount snax.creator platform2 SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV -p snax.creator@active --stake-cpu="15000.0000 SNAX" --stake-net="10000.0000 SNAX" --buy-ram-kbytes=1000

clisnax push action snax setplatforms '[[{"account": "platform1", "weight": 0.995, "period": 2, "quotas": {  "cpu_weight": 150000000, "net_weight": 150000000, "ram_bytes": 10000000 }  }, {"account": "platform2", "weight": 0.005, "period": 1, "quotas": {  "cpu_weight": 150000000, "net_weight": 150000000, "ram_bytes": 10000000 }  }, {"account": "testacc2", "weight": 0, "period": 1, "quotas": {  "cpu_weight": 150000000, "net_weight": 150000000, "ram_bytes": 10000000 }  }]]' -p snax@active

clisnax push action snax setparams '[{   "max_block_net_usage": 1048576,"target_block_net_usage_pct": 1000,"max_transaction_net_usage": 524288,"base_per_transaction_net_usage": 12,"net_usage_leeway": 500,"context_free_discount_net_usage_num": 20,"context_free_discount_net_usage_den": 100,"max_block_cpu_usage": 150000000,"target_block_cpu_usage_pct": 1000,"max_transaction_cpu_usage": 15000000,"min_transaction_cpu_usage": 100, "resources_market_open": 1, "privileged_contracts": 0,"contract_owner":1,"platform_lock_duration":0,"max_transaction_lifetime": 15000,"deferred_trx_expiration_window": 5000,"max_transaction_delay": 3888000,"max_inline_action_size": 4096,"max_inline_action_depth": 4,"max_authority_depth": 6,"max_ram_size": "68719476736", "platforms": [{"account": "platform1", "weight": 0.995, "period": 2 }, {"account": "platform2", "weight": 0.005, "period": 1 }, {"account": "testacc2", "weight": 0, "period": 1 }] }]' -p snax@active

clisnax push action snax.token issue '["testacc1", "1000.0000 SNAX", ""]' -p snax@active
clisnax push action snax buyrambytes '["testacc1", "test.transf", 5000]' -p testacc1@active
clisnax push action snax.token issue '["test.transf", "1000.0000 SNAX", ""]' -p snax@active
clisnax push action snax setparams '[{   "max_block_net_usage": 1048576,"target_block_net_usage_pct": 1000,"max_transaction_net_usage": 524288,"base_per_transaction_net_usage": 12,"net_usage_leeway": 500,"context_free_discount_net_usage_num": 20,"context_free_discount_net_usage_den": 100,"max_block_cpu_usage": 150000000,"target_block_cpu_usage_pct": 1000,"max_transaction_cpu_usage": 15000000,"min_transaction_cpu_usage": 100, "resources_market_open": 1, "privileged_contracts": 0,"contract_owner":1,"platform_lock_duration":0,"max_transaction_lifetime": 15000,"deferred_trx_expiration_window": 5000,"max_transaction_delay": 3888000,"max_inline_action_size": 4096,"max_inline_action_depth": 4,"max_authority_depth": 6,"max_ram_size": "68719476736" }]' -p snax@active
clisnax set contract platform1 ../build/contracts/platform -p platform1@owner
clisnax set account permission platform1 active '{"threshold": 1,"keys": [{"key": "SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","weight": 1}],"accounts": [{"permission":{"actor":"platform1","permission":"snax.code"},"weight":1}]}' owner -p platform1@owner
clisnax set contract platform2 ../build/contracts/platform -p platform2@owner
clisnax set account permission platform2 active '{"threshold": 1,"keys": [{"key": "SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","weight": 1}],"accounts": [{"permission":{"actor":"platform2","permission":"snax.code"},"weight":1}]}' owner -p platform2@owner
clisnax set contract testacc1 ../build/contracts/platform -p testacc1@owner
