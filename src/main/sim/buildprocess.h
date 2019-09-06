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

    auto
  buildNormPath(
    double mu, double vol, double begin, double end, double dt
  ) {
      auto n = norm(mu, vol);
      return run_over<
          decltype(n),
          std::vector<pt_type>
      >(n, begin, end, dt);
  }

   auto 
  buildLogNormalPath(
    double level, double mu, double vol, double begin, double end, double dt
  ) {
    auto ln = lognorm(level, mu, vol);
    return run_over<
            decltype(ln),
            std::vector<pt_type>
          >(ln, begin, end, dt);
  }
}
