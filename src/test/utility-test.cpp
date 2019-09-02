#include "catch2/catch.hpp"

#include "utility/interval.h"


SCENARIO ("interval can be used for traversal std algorithms", "[utility]") {
  using namespace vol::utility;

  typedef interval<double> interval_d;

  WHEN("interval is created") {
    interval_d period(iterator<double>(0.2, 0.05), iterator<double>(0.1, 0.05));
    interval_d period2(iterator<double>(0.1, 0.05), iterator<double>(0.2, 0.05));
      
    THEN("begin <= end") {
      CHECK(period.begin().loc_ < period.end().loc_);
      CHECK(period2.begin() < period2.end());
    }
  
    WHEN("iterator ptr is incremented") {

    }
  }
}
