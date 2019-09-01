#include <iostream>
#include <range/v3/view/zip.hpp>

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
    auto fn = vol::generator::normal(1, 1);
    auto lfn = vol::generator::lognormal(1,2);

    THEN("No two calls to the function are identical") {
      REQUIRE(fn() != fn());
      REQUIRE(lfn() != lfn());
    }
  }

  WHEN("a constant test generator is constructed") {
    auto const1 = vol::generator::constant(1.0);
    auto const2 = vol::generator::constant(2.0);

    THEN("each call to the generator is identical") {
      REQUIRE(const1() == const1());
      REQUIRE(const2() == const2());
      REQUIRE(const1() == 1);
      REQUIRE(const2() == 2);
    }
  }

  WHEN("generators are constructed with a seed sequence of length n") {
    std::seed_seq seeds{1, 2, 3, 4, 5};
    auto norm1 = vol::generator::normal(0.0, 1.0, seeds);
    auto norm2 = vol::generator::normal(0.0, 1.0, seeds);
    auto lognorm1 = vol::generator::lognormal(0.0, 1.0, seeds);
    auto lognorm2 = vol::generator::lognormal(0.0, 1.0, seeds);
    
    //first 5 are normal, secodn 5 are lonormal
    auto refs = {-0.310661, -0.733208, 0.220453, -1.55703, -0.306272, 
      0.732962, 0.480365, 1.24664, 0.210762, 0.736187};

    std::vector<double> outs1, outs2;
    std::generate_n(back_inserter(outs1), 5, norm1);
    std::generate_n(back_inserter(outs1), 5, lognorm1);
    std::generate_n(back_inserter(outs2), 5, norm2);
    std::generate_n(back_inserter(outs2), 5, lognorm2);
    
    THEN("the first n draws from the generator are deterministic") {
      for (auto [out1, out2, ref]: ranges::views::zip(outs1, outs2, refs)) {
        REQUIRE_THAT( out1, Catch::WithinAbs(ref, 1.e-5) );
        REQUIRE_THAT( out2, Catch::WithinAbs(ref, 1.e-5) );
      }
    }

    std::generate_n(back_inserter(outs1), 10, norm1);
    std::generate_n(back_inserter(outs1), 10, lognorm1);
    std::generate_n(back_inserter(outs2), 10, norm2);
    std::generate_n(back_inserter(outs2), 10, lognorm2);
    
    THEN("samples from the seeded generators are the same thereafter.") {
      for(auto [out1, out2]: ranges::views::zip(outs1, outs2)) {
        REQUIRE_THAT( out1, Catch::WithinAbs(out2, 1.e-10) );
      }
    }
  }

}
