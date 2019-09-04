#pragma once

#include "utility/lambda.h"

namespace test {
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

    path_type 
  buildLogNormalPath(
    double level, double mu, double vol, double begin, double end, double dt
  ) {
    path_type result;
    auto lognorm = vol::proc::lognorm(level, mu, vol);

    for (auto t = begin; t < end; t += dt) {
      result.push_back(std::make_pair(t, lognorm(t)));
    }
    return result;
  }

  auto buildAsian(double begin, double end, double dt) {
    return vol::market::asian::asianing<
             path_type,
             decltype(identity), 
             decltype(identity)
           >(identity, identity, begin, end, dt);
  }
}
