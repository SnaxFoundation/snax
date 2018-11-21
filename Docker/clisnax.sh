#!/bin/bash

# Usage:
# Go into cmd loop: sudo ./clisnax.sh
# Run single cmd:  sudo ./clisnax.sh <clisnax paramers>

PREFIX="docker-compose exec snaxnoded clisnax"
if [ -z $1 ] ; then
  while :
  do
    read -e -p "clisnax " cmd
    history -s "$cmd"
    $PREFIX $cmd
  done
else
  $PREFIX "$@"
fi
