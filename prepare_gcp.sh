#!/bin/bash
clear
sudo apt update
sudo apt -y install build-essential git cmake openssl libssl-dev libz-dev pkg-config libzstd-dev
git clone https://github.com/tejas-shivanand-mane/NuRaft.git
cd NuRaft/
./prepare.sh 
cp CMakeListsUbuntu.txt CMakeLists.txt
mkdir build
cd build/
cmake ..
make -j4
