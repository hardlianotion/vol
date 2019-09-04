#include <array>
#include "catch2/catch.hpp"

#include "utility/lambda.h"


inline auto test_constant (double c) {
  return [c] (double) -> double {
    return c;
  };
}

inline auto test_constant_2d (double c) {
  return [c] (double) -> std::vector<double> {
    return {c, c};
  };
}

inline auto test_linear (double c, double r) {
  return [c, r] (double x) -> double {
    return c + x * r;
  };
}

inline auto test_linear_2d (double c, double r) {
  return [c, r] (std::vector<double> x) -> double {
    return x[0] * c + x[1] * r;
  };
}

SCENARIO ("Function composition allows caching in the framework", "[compose]") {
  using namespace vol::utility;
  
  auto constant = test_constant(5.);
  auto constant_2d = test_constant_2d(5.);
  auto linear = test_linear(5., 4.);
  auto linear_2d = test_linear_2d(5., 4.);
  
  WHEN("if we compose linear_2d and constant_2d") {
    auto test = compose(linear_2d, constant_2d);

    THEN ("compose(linear_2d, constant_2d) delivers the composition of linear_2d and composition_2d") {
      CHECK_THAT(test(0.4), Catch::WithinAbs(linear_2d(constant_2d(0.4)),1e-6));
    }
  }

  WHEN("we aggregate linear and constant") {
    auto test = aggregate(linear, constant);
    
    THEN("aggregate(linear, constant) runs the same value through") {
      std::array<double, 2> output = test(0.4);
      std::array<double, 2> reference = {linear(0.4), constant(0.4)};

      CHECK_THAT( output[0], Catch::WithinAbs(reference[0], 1e-6 ));
      CHECK_THAT( output[1], Catch::WithinAbs(reference[1], 1e-6 ));
    }
  }
}

