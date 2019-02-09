# !/bin/bash

snaxnode --verbose-http-errors --delete-all-blocks -e -p snax \
 --data-dir=/tmp/snaxnode_data \
 --http-validate-host=false \
 --max-transaction-time=1000 \
 --contracts-console \
 --signature-provider SNAX6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV=KEY:5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3 \
 --plugin snax::producer_plugin \
 --plugin snax::history_plugin \
 --plugin snax::chain_api_plugin \
 --plugin snax::history_api_plugin \
 --plugin snax::http_plugin \
 --http-server-address=0.0.0.0:8888 \
 --access-control-allow-origin=*
