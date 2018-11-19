#!/bin/bash
#
# snax-tn_bounce is used to restart a node that is acting badly or is down.
# usage: snax-tn_bounce.sh [arglist]
# arglist will be passed to the node's command line. First with no modifiers
# then with --hard-replay-blockchain and then a third time with --delete-all-blocks
#
# the data directory and log file are set by this script. Do not pass them on
# the command line.
#
# in most cases, simply running ./snax-tn_bounce.sh is sufficient.
#

pushd $SNAX_HOME

if [ ! -f programs/snaxnode/snaxnode ]; then
    echo unable to locate binary for snaxnode
    exit 1
fi

config_base=etc/snax/node_
if [ -z "$SNAX_NODE" ]; then
    DD=`ls -d ${config_base}[012]?`
    ddcount=`echo $DD | wc -w`
    if [ $ddcount -ne 1 ]; then
        echo $HOSTNAME has $ddcount config directories, bounce not possible. Set environment variable
        echo SNAX_NODE to the 2-digit node id number to specify which node to bounce. For example:
        echo SNAX_NODE=06 $0 \<options\>
        cd -
        exit 1
    fi
    OFS=$((${#DD}-2))
    export SNAX_NODE=${DD:$OFS}
else
    DD=${config_base}$SNAX_NODE
    if [ ! \( -d $DD \) ]; then
        echo no directory named $PWD/$DD
        cd -
        exit 1
    fi
fi

bash $SNAX_HOME/scripts/snax-tn_down.sh
bash $SNAX_HOME/scripts/snax-tn_up.sh "$*"
