#!/bin/bash
cwd=`pwd`
unzip yosys.zip -d ~/
unzip z3.zip -d ~/
unzip boolector.zip -d ~/
unzip cosaEnv.zip -d ~/
unzip CoSA.zip -d ~/
unzip cvc4.zip -d ~/
unzip grain.zip -d ~/
sudo cp ~/grain/libtinfo.so.5 /usr/lib
sudo ldconfig -n /usr/lib
cd ~/yosys
echo 'install yosys...'
sudo make install
cd ~/z3
sudo 'install z3...'
sudo make install


