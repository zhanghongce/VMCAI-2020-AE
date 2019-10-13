#!/bin/bash
echo '-------------- Test Install ---------------'
cd testcases/test/
bash buildAll.sh
cd build
./ITESTtest

