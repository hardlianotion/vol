#pragma once 
#include "market/market.h"
#include "sim/buildprocess.h"


namespace vol::market {

  auto buildAsian(double begin, double end, double dt) {
    using vol::proc::identity;
    return vol::market::asian::asianing<
             vol::proc::path_type,
             decltype(identity), 
             decltype(identity)
           >(identity, identity, begin, end, dt);
  }
}
