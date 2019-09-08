#include "catch2/catch.hpp"

#include <iostream>

#include "utility/utility.h"


inline auto test_const(double c) {
  return [c] (double t) {
    return c;
  };
}

inline auto test_lin(double c, double r) {
  return [c,r] (double t) {
    return c + r * t;
  };
}

SCENARIO ("transform applies collections of operations to data", "[transform]") {
  WHEN("a list of functions is applied to an operand") {
    std::vector<
      std::function<double(double)>
    > functions = {test_const(5.), test_lin(4., 1.)};
   
    functions.begin()->operator()(3.); 
    std::vector<double> results;
    vol::utility::transform(
      functions.begin(), 
      functions.end(), 
      std::back_inserter(results), 3.);
    
    std::vector<double> ref = {functions[0](3.), functions[1](3.)};
    
    THEN("the results are vector of the operation results.") {
      CHECK_THAT( results, Catch::Approx(ref).epsilon(1.e-5) );
    }
  }
}


