#pragma once

#include "utility/lambda.h"
#include "sim/process.h"

namespace vol::proc {
  auto identity = vol::utility::build_identity();

  typedef std::pair<double, double> pt_type;
  typedef std::vector<std::pair<double, double>> path_type;

    path_type 
  buildConstPath(
    double level, double begin, double end, double dt
  ) {
    path_type result;
    for (auto t = begin; t < end; t += dt) {
      result.push_back(std::make_pair(t, level));
    }
    return result;
  }

    path_type 
  buildLinearPath(
    double level, double scale, double begin, double end, double dt
  ) {
    path_type result;
    for (auto t = begin; t < end; t += dt) {
      result.push_back(std::make_pair(t, level + scale * t));
    }
    return result;
  }

    path_type 
  buildNormPath(
    double mu, double vol, double begin, double end, double dt
  ) {
    path_type result;
    auto norm = vol::proc::norm(mu, vol);

    for (auto t = begin; t < end; t += dt) {
      result.push_back(std::make_pair(t, norm(t)));
    }
    return result;
  }

   auto 
  buildLogNormalPath(
    double level, double mu, double vol, double begin, double end, double dt
  ) {
    return run_over<
            decltype(lognorm(level, mu, vol)), 
            std::vector<pt_type>
          >(lognorm(level, mu, vol), begin, end, dt);
  }
}
