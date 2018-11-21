#!/bin/bash
##########################################################################
# This is the SNAX automated install script for Linux and Mac OS.
# This file was downloaded from https://github.com/SnaxFoundation/snax
#
# Copyright (c) 2017, Respective Authors all rights reserved.
#
# After June 1, 2018 this software is available under the following terms:
#
# The MIT License
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# https://github.com/SnaxFoundation/snax/blob/master/LICENSE.txt
##########################################################################

if [ "$(id -u)" -ne 0 ]; then
        printf "\n\tThis requires sudo. Please run with sudo.\n\n"
        exit -1
fi

   CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
   if [ "${CWD}" != "${PWD}" ]; then
      printf "\\n\\tPlease cd into directory %s to run this script.\\n \\tExiting now.\\n\\n" "${CWD}"
      exit 1
   fi

   BUILD_DIR="${PWD}/build"
   CMAKE_BUILD_TYPE=Release
   TIME_BEGIN=$( date -u +%s )
   INSTALL_PREFIX="/usr/local/snax"
   VERSION=1.2

   txtbld=$(tput bold)
   bldred=${txtbld}$(tput setaf 1)
   txtrst=$(tput sgr0)

   create_symlink() {
      pushd /usr/local/bin &> /dev/null
      ln -sf ../snax/bin/$1 $1
      popd &> /dev/null
   }

   create_cmake_symlink() {
      mkdir -p /usr/local/lib/cmake/snax
      pushd /usr/local/lib/cmake/snax &> /dev/null
      ln -sf ../../../snax/lib/cmake/snax/$1 $1
      popd &> /dev/null
   }

   install_symlinks() {
      printf "\\n\\tInstalling SNAX Binary Symlinks\\n\\n"
      create_symlink "clisnax"
      create_symlink "snax-abigen"
      create_symlink "snax-launcher"
      create_symlink "snax-s2wasm"
      create_symlink "snax-wast2wasm"
      create_symlink "snaxcpp"
      create_symlink "kxd"
      create_symlink "snaxnode"
   }

   if [ ! -d "${BUILD_DIR}" ]; then
      printf "\\n\\tError, snax_build.sh has not ran.  Please run ./snax_build.sh first!\\n\\n"
      exit -1
   fi

   ${PWD}/scripts/clean_old_install.sh
   if [ $? -ne 0 ]; then
      printf "\\n\\tError occurred while trying to remove old installation!\\n\\n"
      exit -1
   fi

   if ! pushd "${BUILD_DIR}"
   then
      printf "Unable to enter build directory %s.\\n Exiting now.\\n" "${BUILD_DIR}"
      exit 1;
   fi

   if ! make install
   then
      printf "\\n\\t>>>>>>>>>>>>>>>>>>>> MAKE installing SNAX has exited with the above error.\\n\\n"
      exit -1
   fi
   popd &> /dev/null

   install_symlinks
   create_cmake_symlink "snax-config.cmake"

   printf "\\tQQRBRBRBRQRBRQQQRBRBRQRBQBBBQBBBQBBBBBBBBBBBQBRBQQQQRQRBRQQBRBRBQQQQRQRBQBM\\n"
   printf "\\tBMBRBQQRQRQRQQQQBRBQQRBBBBBBBb17:....i7LKgBBBBBBBRBRBRQQQQQRBRBRBQQRQQQQBRQ\\n"
   printf "\\tQQQQRBQBQBRQRQRBRQRQRBQBQMJ.     ..::...  .LKBQBBBQBQBRQQQRBRQQBRQRQQQRBQBM\\n"
   printf "\\tQMQRBRBQQQBRBRBRBQBQBQBP.     ir7rriiii:i::. .YZBBBQBRQQBRQQQQBRBRBRBQQRBQQ\\n"
   printf "\\tRQQQQQRBRBRQRQRQRBQBQB:    . rvririiiiiiii:i::. 7DBBBBRBRBRQQQRQRBRBRQRQRBM\\n"
   printf "\\tBMBRQRBRBRBRBRBRBRBQB   ...  ivii:i::.::iiiii:i:. LBBQBQBQQRBRBRQQQRBRBRBRQ\\n"
   printf "\\tQQRBRQRBRQRQRBQBRBBB:  ..... .7r:..::i:. ::iiiii:: .EBBBQBRBRQRBQQRQRQRBRBM\\n"
   printf "\\tBMBRBRQRBRBRQRQRQRBB   ...... .i75BBBQBQb:.:iiiiiii. XBBBRBRBQQRQQQQQRBQQQQ\\n"
   printf "\\tRQQQRBQBQQRQQBRBQQQBU  .......  QBBBQBBBBB7.:i:iiiii. bBBBRQRQQQQQRQRQRQRBR\\n"
   printf "\\tBMBRBRBRQQQRBRBQQQBBBr  ..... . .QBQBQBQBBBY::iiiiiii..BBQQRBQBRBRQRBQQRQRQ\\n"
   printf "\\tQQRQQQRQRQQQRBRQRBQBBB:    .....  bBBBRQQBQBr7ii:iiiii YQBRQRBRQRQRBRBRQRBM\\n"
   printf "\\tBMBRBRQQBRQRQQBRBQQRBBMi:   .....  UBQBQQQBBb:Y:iiiii:: BBBRQRBRBQBRBRBQQQQ\\n"
   printf "\\tQQRBRBQQQBRBQQQQRQQBQBQbis.  . ...  YBBBQQQBBrvv:iiiii: SBQBRBQBRQRBRBRQQBM\\n"
   printf "\\tBMBRBRQRBQBRQRBRQQQRQQBB5iu7  . ...  vBQBQBBBivLi:iiiii.JBBQQQBQQRQRBRQRBQQ\\n"
   printf "\\tQQRQRBRQRBRQRBQQRQRBRBQBQU:jj:  ....  vBQBQBDi7siiiiii:.7BBBQBQQQBQBRQQQRQM\\n"
   printf "\\tBMBRQRBQBRQRQRBRQRBQBRBQBBu:su7   ...  sBBBQ5:LLr:iiiii.vQBRQRBQQRBRBQBQBRQ\\n"
   printf "\\tQQRQRBRQQQRQRBQBRQQBRBRBQBBY:YjJ   ...  5BBB7r7siiiiii:.sBQBQQRBRBRQRBRBRQM\\n"
   printf "\\tBMBRBRBRQQBRBQBRBRBRQRQQQQBB7:LJ2   ...  MBM:7LLiiiiiii PQBRQRQRBQBRBQQQBQQ\\n"
   printf "\\tQQRQQQQQRQQBRBRBRBQBRQQQRQQBB7iLJ2   ...  QurLLL:iiiii:.QBQBQBRBQBRBRQRQQBM\\n"
   printf "\\tBRBRQQQRBQBRBRQRQQQQQRQQBRBBBQirLJ2  .... .LLvsii:iii:.7BQBQBRBRBRBRQQQQQRQ\\n"
   printf "\\tRQQBQBRQQQRBQBRQRBRBRBQQRBQBBBP:7Yjj  .. . 7JJ7iiiiiii dQBQBRQRQRQQQRQRBQQM\\n"
   printf "\\tBMBRBRQRBQQRQRQRQRBQBRQRBRQRBBBLi7Yur . .  .IY::iiiii.7QBRBRBRBQBRBQBQBQQRQ\\n"
   printf "\\tQQRBQQRQRBRBRQQBRQQQRQQBRBRBQBBQi7vs1. ...  Lriiiiii: RBQBQQQQRQQQRBRBRQRBM\\n"
   printf "\\tBMQRQRQRQQQRBRQQBQQRBQQRBRQRBQBBLiLvu7  ... :riiiii: 2BQQQQQQRBRBRQQQQBQBQQ\\n"
   printf "\\tQQRQRQRBQQRQQQRBQQRQRQQBRBRBQBQBX:7LY2  ..  :7iiii: YBBBRQQBRQRQRBQQQBRQQBM\\n"
   printf "\\tBMBQQQQRQQQRBRBQBRBQBRBRBBBBBBBBurL7s1: ... :ri:i: YBBBRQRBRQRBQBRBQBQQRBQQ\\n"
   printf "\\tRQQQRQRQRBQQQBRQRBRBQBQBBBBQdIr.iL7LL2: ..  :7ii. UBBBRBQQQBRBQQQQQQRBRQRBR\\n"
   printf "\\tBMQRQRBQQRBRBQQQBRQRBQBBbr.   .iLvLvs1. ..  ri: :ZBBBRQRBRBRQQBRQRBRQRQRBQQ\\n"
   printf "\\tQQQBRBRQQQRBRBQQRQQBRBBB  .:::isvL7Ls1  .. :r. jBBBBRBRQQBQBRBRQRBRBQBRBQBM\\n"
   printf "\\tBMBRQRQRBQBRBRBQBRQQBRBg.:i:r7svL7LL2:  . ...7QBBQBRBRQRQRQRQRQQBRBRBQBRBQQ\\n"
   printf "\\tRQRQQBRBQBRQRBRQQBRBRBQb:LLYLsvL7LL27     .YZBQBQBRQQQRBQQRQRQRBRBRBRBQBQBM\\n"
   printf "\\tQMQRBRBQBRBRQRBRBRBRQQBd.rv7LvLvYY17     1BQBQBQBQBQQRBRQQQRBQBRBRBQBQBRQRQ\\n"
   printf "\\tQQQBRBRBQQRBRQRQRBRBRBQB5i::irr77r    :bQBBBQBQQRBRQRBRBRBRBQBRBQBRBRQRQRBM\\n"
   printf "\\tQMBRBRQRBRBQBRQRQRBRQRBBBBqYvrri. :7XBBBBQBQBRQRQRBRQRBQQRQRQRBRQRQQQRBRQRQ\\n"
   printf "\\tQQQQRQRQRQQQRQQQRQRBRBQBQBBBQBQBBBBBBBBBQBQQRQQBRQRBRBRQRQQBQQRBRQQQRBRQRBR\\n"

   printf "\\tFor more information:\\n"
   printf "\\tSnax website: https://snax.one\\n"
   printf "\\tSnax Discord channel: https://discord.gg/qygxJAZ\\n"
   printf "\\tSnax Telegram channel: https://t.me/snaxone\\n\\n\\n\\n"
