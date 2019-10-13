#!/bin/bash
echo '-------------------- Build ILAng ---------------------'
cd ILA-Tool
bash rebuild
cd build
echo '-------------------- Install ILAng ---------------------'
sudo make install

