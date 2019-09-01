#!/bin/sh

apt-get update 
apt-get install -y git
apt-get install -y gcc g++
apt-get install -y cmake
rm -rf /var/lib/apt/lists
git clone --branch v2.9.2 --depth 1 https://github.com/catchorg/Catch2.git

#install test framework
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
cmake --build build/ --target install

#install gcem and stats
git clone --depth 1  https://github.com/kthohr/gcem.git
git clone --depth 1 https://github.com/kthohr/stats.git

mv gcem/include/* /usr/local/include
mv stats/include/* /usr/local/include

#install ranges
cd ..
git clone --branch 0.9.0 --depth 1 https://github.com/ericniebler/range-v3.git
cd range-v3
cmake -Bbuild -H. -DBUILD_TESTING=OFF -DRANGE_V3_TESTS=OFF -DRANGE_V3_EXAMPLES=OFF -DRANGE_V3_PERF=OFF -DRANGE_V3_DOCS=OFF -DRANGE_V3_HEADER_CHECKS=OFF
cmake --build build/ --target install

