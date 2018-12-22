#!/bin/sh
cd /opt/snax/bin

if [ ! -d "/opt/snax/bin/data-dir" ]; then
    mkdir /opt/snax/bin/data-dir
fi

if [ -f '/opt/snax/bin/data-dir/config.ini' ]; then
    echo
  else
    cp /config.ini /opt/snax/bin/data-dir
fi

if [ -d '/opt/snax/bin/data-dir/contracts' ]; then
    echo
  else
    cp -r /contracts /opt/snax/bin/data-dir
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
    CONFIG_DIR="--config-dir=/opt/snax/bin/data-dir"
else
    CONFIG_DIR=""
fi

exec /opt/snax/bin/snaxnode $CONFIG_DIR "$@"
