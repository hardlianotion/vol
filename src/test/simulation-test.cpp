#include "catch2/catch.hpp"
#include "sim/simulation.h"

SCENARIO ("lambdas can be passed to variables.", "[simulation]") {
  WHEN("a lambda is passed to an auto var") {
    auto fn = [](int x) {return x + 1;};

    THEN("The var can be used as a function") {
      REQUIRE(fn(1) == 2);
    }
  }
}
