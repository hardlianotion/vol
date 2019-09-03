#include "catch2/catch.hpp"

#include "utility/interval.h"
#include "sim/simulation.h"
#include <iostream>

SCENARIO ("transform applies collections of operations to data", "[transform]") {
  using namespace vol::utility;

  WHEN("a list of functions is applied to an operand") {
    using namespace vol::proc;
    std::vector<
      std::function<double(double)>
    > functions = {constant(5.), linear(4., 1.)};
   
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

SCENARIO ("interval can be used for traversal std algorithms", "[utility]") {
  using namespace vol::utility;

  typedef interval<double> interval_d;

  WHEN("interval is created") {
    interval_d period(iterator<double>(0.2, 0.05), iterator<double>(0.1, 0.05));
    interval_d period2(iterator<double>(0.1, 0.05), iterator<double>(0.2, 0.05));
      
    THEN("begin <= end") {
      CHECK(period.begin() < period.end());
      CHECK(period2.begin() < period2.end());
    }
  
    WHEN("lhs and rhs have the same value and we apply ++ to rhs") {
      iterator<double> lhs = period.begin();
      iterator<double> rhs = period.begin();
      ++rhs; 
      
      THEN("lhs < rhs") {
        CHECK(lhs < rhs);  
      }
    }
    
    WHEN("lhs and rhs have the same value and we apply -- to rhs") {
      iterator<double> lhs = period.begin();
      iterator<double> rhs = period.begin();
      --rhs;

      THEN("rhs < lhs") {
        CHECK(rhs < lhs);  
      }
    }

    WHEN("lhs and rhs have the same value") {
      iterator<double> lhs = period.begin();
      iterator<double> rhs = period.begin();

      THEN("distance(lhs, rhs) = 0") {
        CHECK_THAT(distance(lhs, rhs), Catch::WithinAbs(0., 1e-6));
        CHECK_THAT(distance(rhs, lhs), Catch::WithinAbs(0., 1e-6));
      }

      WHEN("if we operate ++ on rhs 5 times") {
        ++rhs; ++rhs; ++rhs; ++rhs; ++rhs;
        THEN("lhs and rhs are 5. apart") {
          CHECK_THAT(distance(lhs, rhs), Catch::WithinAbs(5., 1e-6));
        }
      }
      
      WHEN("if we operate ++ on lhs 5 times") {
        ++lhs; ++lhs; ++lhs; ++lhs; ++lhs;
        THEN("lhs and rhs are 5. apart") {
          CHECK_THAT(distance(lhs, rhs), Catch::WithinAbs(-5., 1e-6));
        }
      }
    }
  }
}

