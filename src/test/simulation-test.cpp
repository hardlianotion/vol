#include "catch2/catch.hpp"
#include "sim/simulation.h"

SCENARIO ("Simulation pipelines are composed functions.", "[simulation]") {
  WHEN("a lambda is passed to an auto var") {
    auto fn = [](int x) {return x + 1;};

    THEN("The var can be used as a function") {
      REQUIRE(fn(1) == 2);
    }
  }

  WHEN("a generator is constructed") {
    auto fn = vol::Generator::normal(1, 1);
    auto lfn = vol::Generator::lognormal(1,2);

    THEN("No two calls to the function are identical") {
      REQUIRE(fn() != fn());
      REQUIRE(lfn() != lfn());
    }
  }

  WHEN("a constant test generator is constructed") {
    auto const1 = vol::Generator::constant(1.0);
    auto const2 = vol::Generator::constant(2.0);

    THEN("each call to the generator is identical") {
      REQUIRE(const1() == const1());
      REQUIRE(const2() == const2());
      REQUIRE(const1() == 1);
      REQUIRE(const2() == 2);
    }

  }

}
