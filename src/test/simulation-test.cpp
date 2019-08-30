#include "catch2/catch.hpp"
#include "sim/simulation.h"

SCENARIO ("Simulation pipelines are composed functions.", "[simulation]") {
  WHEN("a lambda is passed to an auto var") {
    auto fn = [](int x) {return x + 1;};

    THEN("The var can be used as a function") {
      REQUIRE(fn(1) == 2);
    }
  }

  WHEN("Generator::normal constructs a function.") {
    auto fn = vol::Generator::normal(1, 1);

    THEN("No two calls to the function are identical") {
      double first = fn();
      double second = fn();
      REQUIRE(first != second);
    }
  }
}
