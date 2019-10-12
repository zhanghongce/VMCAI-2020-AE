#!/bin/bash

DEPPACKS="flex bison libboost-all-dev verilator libtcl8.6 libreadline-dev tcl8.6-dev tcl-dev python3-venv libgmp3-dev libmpfr-dev libmpc-dev subversion libncurses-dev"
cd packages
apt-get --print-uris install $DEPPACKS | grep "^'" | sed "s/^'\([^']*\)'.*$/\1/g" > all.deps
for i in $(cat all.deps) ; do wget -nv $i ; done

