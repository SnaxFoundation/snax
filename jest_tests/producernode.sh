# !/bin/bash

snaxnode \
--max-irreversible-block-age -1 \
--contracts-console \
--hard-replay \
--blocks-dir /tmp/snaxnode_data/blocks \
--config-dir /tmp/snaxnode_data \
--data-dir /tmp/snaxnode_data \
--chain-state-db-size-mb 1024 \
--http-server-address 127.0.0.1:8001 \
--p2p-listen-endpoint 127.0.0.1:9001 \
--max-clients 10 \
--p2p-max-nodes-per-host 10 \
--enable-stale-production \
--producer-name test2 \
--private-key '["SNAX82vokU8bz112PQueGtpG6qkW2tXXrAGPjsVjsNEkZRXaGH4xTe" ,"5JXpXedBoLQbktK5CP6rv2J5HeeLrnpi4aU5dUgb34vjNSefftb"]' \
--plugin snax::http_plugin \
--plugin snax::chain_api_plugin \
--plugin snax::producer_plugin \
