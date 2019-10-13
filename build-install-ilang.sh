#!/bin/bash
echo '-------------------- Build ILAng ---------------------'
cd ILA-Tools
bash rebuild
cd build
echo '-------------------- Install ILAng ---------------------'
sudo make install

