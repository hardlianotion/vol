
#include <iostream>
#include <array>
#include <tuple>

#include "catch2/catch.hpp"

#include "sim/simulation.h"
#include "stats/statistics.h"


SCENARIO ("samples are used to summarise simulations..", "[sample]") {
  WHEN("a sample is constructed") {
    using namespace vol::stats;

    std::vector<double> sample = {0., 1., 2., 3., 4.};

    THEN("sample_sums delivers sample size, mean and variance") {
      auto summary = variance(sample.begin(), sample.end());

      CHECK_THAT(std::get<0>(summary), Catch::WithinAbs(5u, 1.e-6));
      CHECK_THAT(std::get<1>(summary), Catch::WithinAbs(2., 1.e-6));
      CHECK_THAT(std::get<2>(summary), Catch::WithinAbs(2.5, 1.e-6));
    }
  }

  WHEN("a perfectly negatively correlated 2d sample is constructed") {
    using namespace vol::stats;
    std::vector<std::array<double, 2>> sample_pair = {{0., 4.},
                                                      {1., 3.},
                                                      {2., 2.},
                                                      {3., 1.},
                                                      {4., 0.}};
    THEN("sample_sums delivers sample size, mean and variance") {
      auto summary = covariance(sample_pair.begin(), sample_pair.end());

      CHECK(std::get<0>(summary) == 5u);
      CHECK_THAT(std::get<1>(summary), Catch::WithinAbs(2., 1.e-6));
      CHECK_THAT(std::get<2>(summary), Catch::WithinAbs(2., 1.e-6));
      CHECK_THAT(std::get<3>(summary), Catch::WithinAbs(2.5, 1.e-6));
      CHECK_THAT(std::get<4>(summary), Catch::WithinAbs(-2.5, 1.e-6));
      CHECK_THAT(std::get<5>(summary), Catch::WithinAbs(2.5, 1.e-6));
    }

    WHEN("a control variate is presented") {
      double c_mean = 2.1;

      THEN("we can calculate a mean and variance.") {
        auto result = summary(sample_pair.begin(), sample_pair.end(), c_mean);

        CHECK_THAT(std::get<1>(result), Catch::WithinAbs(1.9, 1.e-6));
        CHECK_THAT(std::get<2>(result), Catch::WithinAbs(0., 1e-6));
      }
    }
  }

  WHEN("an imperfectly  correlated 2d sample is constructed") {
    using namespace vol::stats;
    std::vector<std::array<double, 2>> sample_pair = {{0., 0.},
                                                      {1., 3.},
                                                      {2., 4.},
                                                      {3., 0.},
                                                      {4., 8.}};
    THEN("sample_sums delivers sample size, mean and variance") {
      auto summary = covariance(sample_pair.begin(), sample_pair.end());

      CHECK(std::get<0>(summary) == 5u);
      CHECK_THAT(std::get<1>(summary), Catch::WithinAbs(2., 1.e-6));
      CHECK_THAT(std::get<2>(summary), Catch::WithinAbs(3., 1.e-6));
      CHECK_THAT(std::get<3>(summary), Catch::WithinAbs(2.5, 1.e-6));
      CHECK_THAT(std::get<4>(summary), Catch::WithinAbs(3.25, 1.e-6));
      CHECK_THAT(std::get<5>(summary), Catch::WithinAbs(11., 1.e-6));
    }

    WHEN("a control variate is presented") {
      double c_mean = 2.1;

      THEN("we can calculate a mean and variance.") {
        auto result = summary(sample_pair.begin(), sample_pair.end(), c_mean);

        CHECK_THAT(std::get<1>(result), Catch::WithinAbs(1.4422243, 1.e-6));
        CHECK_THAT(std::get<2>(result), Catch::WithinAbs(2.6966205, 1e-6));
      }
    }
  }
}

SCENARIO("histogram provides quantile summary of a sample", "[histogram]") {
  using namespace vol::stats;

  WHEN("sample passed to histogram builder with acc and quant params") {
    double acc = 1.e-2, quant = 0.1;
    std::vector<double> sample = {1., 2., 5., 9., 8., 6., 6.,
                                  5., 4., 2., 2., 2., 1., 6.,
                                  8., 9., 5., 5., 1., 3., 8.,
                                  6., 0., 0., 2., 3., 4., 7.};

    auto inc_request = histogram_request(acc, quant, hist_type::INCREMENTAL);
    auto cum_request = histogram_request(acc, quant, hist_type::CUMULATIVE);

    auto hist_sketch = build_histogram_sketch(inc_request, sample);
    std::cout << "incremental: " << std::endl;
    auto hist = std::get<2>(build_histogram(inc_request, hist_sketch));
    std::cout << "cumulative: " << std::endl;
    auto cum_hist = std::get<2>(build_histogram(cum_request, hist_sketch));

    THEN("histogram builder outputs histogram of size [1/quant + 1]") {
      CHECK(hist.size() == std::floor(1. / quant + 0.5));
      CHECK(cum_hist.size() == std::floor(1. / quant + 0.5));
    }

    THEN("histogram histogram differ from each other by 1. / quant") {
      auto iter = hist.begin();

      for (auto last = iter++; iter != hist.end(); last = iter++) {
        CHECK(fabs(iter->quantile_ - last->quantile_) < 1. / quant);
      }
    }
    THEN("the sum of the distribution histogram counts is the sample size.") {
      size_t count = 0u;

      for(auto item: hist) {
        count += item.count_;
      }

      CHECK(count == sample.size());
    }

    THEN("the last item of the cumulative histogram is the sample size") {
      CHECK(cum_hist.back().count_ == sample.size());
    }

    THEN("the cumulative histogram is monotone increasing wrt quantile.") {
      auto iter = cum_hist.begin();
      for(auto last = iter++; iter != cum_hist.end(); last = iter++){
        CHECK(iter->count_ > last->count_);
      }
    }
  }
}


