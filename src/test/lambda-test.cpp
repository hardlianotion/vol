#include "catch2/catch.hpp"

#include "utility/lambda.h"


inline auto test_constant (double c) {
  return [c] (double) -> std::vector<double> {
    return {c, c};
  };
}

inline auto test_linear (double c, double r) {
  return [c, r] (std::vector<double> x) -> double {
    return x[0] * c + x[1] * r;
  };
}

SCENARIO ("Function composition allows caching in the framework", "[compose]") {
  using namespace vol::utility;
  
  WHEN("if we compose linear and constant") {
    auto constant = test_constant(5.);
    auto linear = test_linear(5., 4.);

    auto test = compose(linear, constant);

    THEN ("linear | constant delivers the composition of linear and composition") {
      CHECK_THAT(test(0.4), Catch::WithinAbs(linear(constant(0.4)),1e-6));
    }
  }
}

