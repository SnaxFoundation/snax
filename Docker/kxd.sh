#!/bin/bash

restore_wallet () {
  WALLET_DIR=$(echo $1 | sed 's/.*=//')
  sleep 10s
  if ls ${WALLET_DIR}/*.wallet 1> /dev/null 2>&1; then
    for WALLET in $(ls ${WALLET_DIR}/ | grep .wallet | sed 's/\..*//g'); do
      clisnax --wallet-url=http://localhost:8900 wallet open --name ${WALLET}
    done
  fi
}

if [ ! "$WALLET_DIR" ]; then
    WALLET_DIR="--wallet-dir=/root/snax-wallet"
else
    WALLET_DIR=""
fi

restore_wallet "${WALLET_DIR}" &

exec /opt/snax/bin/kxd ${WALLET_DIR} "$@"