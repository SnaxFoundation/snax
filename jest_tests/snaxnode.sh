# !/bin/bash

snaxnode --verbose-http-errors --delete-all-blocks -e -p snax \
 --data-dir=/tmp/snaxnode_data \
 --http-validate-host=false \
 --max-transaction-time=1000 \
 --contracts-console \
 --signature-provider 5JXpXedBoLQbktK5CP6rv2J5HeeLrnpi4aU5dUgb34vjNSefftb=KEY:SNAX82vokU8bz112PQueGtpG6qkW2tXXrAGPjsVjsNEkZRXaGH4xTe \
 --plugin snax::producer_plugin \
 --plugin snax::history_plugin \
 --plugin snax::chain_api_plugin \
 --plugin snax::history_api_plugin \
 --plugin snax::http_plugin \
 --http-server-address=0.0.0.0:8888 \
 --access-control-allow-origin=*
