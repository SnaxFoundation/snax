#!/bin/sh
cd /opt/snax

if [ ! -d "/opt/snax/data" ]; then
    mkdir /opt/snax/data
fi

if [ ! -d "/opt/snax/data/blocks" ]; then
    mkdir /opt/snax/data/blocks
fi

if [ ! -d "/opt/snax/data/data-dir" ]; then
    mkdir /opt/snax/data/data-dir
fi

if [ ! -d "/opt/snax/data/configs" ]; then
    mkdir /opt/snax/data/configs
fi

if [ -f '/opt/snax/data/configs/p2p.ini' ]; then
    cat /opt/snax/data/configs/p2p.ini >> /config.ini
fi

if [ ! -f '/opt/snax/data/configs/config.ini' ]; then
    cp /config.ini /opt/snax/data/configs
fi

if [ ! -d '/opt/snax/data/contracts' ]; then
    cp -r /contracts /opt/snax
fi

while :; do
    case $1 in
        --config-dir=?*)
            CONFIG_DIR=${1#*=}
            ;;
        *)
            break
    esac
    shift
done

if [ ! "$CONFIG_DIR" ]; then
    CONFIG_DIR="--config-dir=/opt/snax/data/configs"
else
    CONFIG_DIR=""
fi

if [ ! "$GENESIS_JSON" ]; then
    GENESIS_JSON="--genesis-json=/genesis.json"
else
    GENESIS_JSON=""
fi

DATA_DIR="--data-dir=/opt/snax/data/data-dir"

exec /opt/snax/bin/snaxnode $GENESIS_JSON $CONFIG_DIR $DATA_DIR "$@"