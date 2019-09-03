
#include <iostream>
#include <numeric>

#include "catch2/catch.hpp"
#include "sim/process.h"
#include "sim/simulation.h"


SCENARIO ("Simulation processes are composed lambdas.", "[process]") {

  WHEN("a bm or normal process is run") {
    auto brownian_process = vol::proc::bm();
    auto normal_process = vol::proc::norm(1., 2.);

    std::vector<double> sample;
    THEN("each draw creates a valid sample from a normal distribution") {
      sample.push_back(brownian_process(0.2));
      sample.push_back(brownian_process(0.4));

      REQUIRE(sample.size() == 2u);

      sample.push_back(normal_process(4.5));
      
      REQUIRE(sample.size() == 3u);
      
      double sum = std::accumulate(sample.begin(), sample.end(), 0.);
      
      REQUIRE( sum != std::numeric_limits<double>::quiet_NaN() );
    } 

    THEN("each sample is different from the next sample") {
      REQUIRE(brownian_process(0.2) != brownian_process(0.2));
      REQUIRE(brownian_process(0.4) != brownian_process(0.2));

      REQUIRE(normal_process(0.2) != normal_process(0.2));
      REQUIRE(normal_process(0.4) != normal_process(0.2));
    }
  }

}

