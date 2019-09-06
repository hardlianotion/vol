#!/bin/sh
declare -n var_name=cmake_generator
cmake_generator="Unix Makefiles"
if [[ $# -ge 1  ]]; then
  echo 'usage: ./install-lite.sh <optional cmake-generator-name>'
  echo ''
  echo 'if no generator name given, "Unix Makefiles" generated.'
elif [[ $# -eq 1  ]]; then
  cmake_generator=${CMDLINE[@]}
fi

echo "cmake generator is ${cmake_generator}"

#install test framework
rm -rf Catch2
git clone --branch v2.9.2 --depth 1 https://github.com/catchorg/Catch2.git
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
cmake --build build/ --target install

mkdir -p ../build & cd ../build
rm -rf * & cmake -G "${cmake_generator}" ..

if [[ ${cmake_generator} == *"Unix"* ]]; then
  echo 'Building vol.'
  make
fi
