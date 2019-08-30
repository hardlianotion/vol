#!/bin/sh

apt-get update 
apt-get install -y git
apt-get install -y gcc g++
apt-get install -y cmake
rm -rf /var/lib/apt/lists
git clone https://github.com/catchorg/Catch2.git

cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install
