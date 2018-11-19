#! /bin/bash

binaries=(clisnax
          snax-abigen
          snax-launcher
          snax-s2wasm
          snax-wast2wasm
          snaxcpp
          kxd
          snaxnode
          snax-applesdemo)

if [ -d "/usr/local/snax" ]; then
   printf "\tDo you wish to remove this install? (requires sudo)\n"
   select yn in "Yes" "No"; do
      case $yn in
         [Yy]* )
            if [ "$(id -u)" -ne 0 ]; then
               printf "\n\tThis requires sudo, please run ./snax_uninstall.sh with sudo\n\n"
               exit -1
            fi

            pushd /usr/local &> /dev/null
            rm -rf snax
            pushd bin &> /dev/null
            for binary in ${binaries[@]}; do
               rm ${binary}
            done
            # Handle cleanup of directories created from installation
            if [ "$1" == "--full" ]; then
               if [ -d ~/Library/Application\ Support/snax ]; then rm -rf ~/Library/Application\ Support/snax; fi # Mac OS
               if [ -d ~/.local/share/snax ]; then rm -rf ~/.local/share/snax; fi # Linux
            fi
            popd &> /dev/null
            break;;
         [Nn]* )
            printf "\tAborting uninstall\n\n"
            exit -1;;
      esac
   done
fi
