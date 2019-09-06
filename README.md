
# vol  &nbsp;  &nbsp; &nbsp; [![Codacy Badge](https://api.codacy.com/project/badge/Grade/4df2b8a9b6f0474ca75cc5a7f986588f)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=hardlianotion/vol&amp;utm_campaign=Badge_Grade)![CircleCI](https://circleci.com/gh/hardlianotion/vol.svg?style=svg&circle-token=7b4b779407f176bace72f91599ea41ac3fd4e81d)

Experiments with volatility and options.

## dependencies
Project is built with clang running c++17 and tested on a g++ version with c++17.
Other compilers may work too...

The framework depends on:
*  ~~Eric Niebler's range-v3 0.9.0~~ not at the moment
*  Catch2 v2.9.2
*  ~~kthohr/stats~~ not at the moment
*  ~~kthohr/gcem~~ not at the moment

## build
If you have a unix-like, cmake and a cmake-consistent toolchain, you can run from project root:

`utility/install-lite.sh <optional cmake_generator_name>`

This will download vol dependencies and, if you run it without the optional generator name, with install a
set of unix makefiles and try to build them.  Otherwise, cd to build and

```
cd build
cmake ..
``` 

`build/src/vol-demo` runs a ~~simulation of an asian option price with control variate variance reduction~~ ...something... 
and 
`build\test\vol-tests runs the available tests.

