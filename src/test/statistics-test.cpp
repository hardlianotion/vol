
#include <iostream>

#include <tuple>

#include <range/v3/all.hpp>

#include "catch2/catch.hpp"

#include "sim/simulation.h"


SCENARIO ("samples are used to summarise simulations..", "[sample]") {
  WHEN("a sample is constructed") {
    using namespace vol::stats;

    std::vector<double> sample = {0., 1., 2., 3., 4.};

    THEN("sample_sums delivers sample size, mean and variance") {
      auto summary = variance(sample.begin(), sample.end());

      REQUIRE( std::get<0>(summary) == 5u );
      REQUIRE( std::get<1>(summary) == 2. );
      REQUIRE( std::get<2>(summary) == 2.5 );
    }
  }
  
  WHEN("a 2d sample is constructed") {
    using namespace vol::stats;
    std::vector<double> sample = {0., 1., 2., 3., 4.}; 
    std::vector<double> sample2 = {4., 3., 2., 1., 0.};
    auto sample_pair = ranges::views::zip(sample, sample2);

    THEN("sample_sums delivers sample size, mean and variance") {
      auto summary = covariance(sample_pair.begin(), sample_pair.end());

      REQUIRE( std::get<0>(summary) == 5u );
      REQUIRE( std::get<1>(summary) == 2. );
      REQUIRE( std::get<2>(summary) == 2. );
      REQUIRE( std::get<3>(summary) == 2.5 );
      REQUIRE( std::get<4>(summary) == -2.5 );
      REQUIRE( std::get<5>(summary) == 2.5 );
    }
  }
}

