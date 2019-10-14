#!/bin/bash
echo '-------------------- Build ILAng ---------------------'
unzip ILA-Tools.zip ~/
cd ~/ILA-Tools
bash rebuild
cd build
echo '-------------------- Install ILAng ---------------------'
sudo make install

