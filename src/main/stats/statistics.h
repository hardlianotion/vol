#pragma once

#include <map>

#include "utility/utility.h"


namespace vol::stats {

  typedef std::tuple<size_t, double, double> summary_type;

  template<typename forward_iterator>
  summary_type sample_sums(
    const forward_iterator& begin, const forward_iterator& end
  ) {
    using namespace std;
    size_t size = distance(begin, end);

    return std::accumulate(
      begin, 
      end, 
      std::make_tuple(size, 0., 0.), 
      [] (auto accum, auto next) -> summary_type {
        return {
          get<0>(accum),
          get<1>(accum) + next, 
          get<2>(accum) + next*next
        };});
  }

  /**
   * moment calculation for paired sample.  It is the responsibility of the user 
   * to pair the sample.
   * returns size, sx, sy, sxx, sxy, syy.
   */
  //FIXME - hack.  want any qualifying container iterator
  typedef std::tuple<
    size_t, double, double, 
    double, double, double> summary_type_2d;

  template <typename forward_iterator>
  summary_type_2d sample_sums_2d(
    const forward_iterator& begin, const forward_iterator& end
  ) {
    using namespace std;
    size_t size = distance(begin, end);
    return std::accumulate(
      begin, 
      end, 
      std::make_tuple(size, 0., 0., 0., 0., 0.), 
      [](auto accum, auto next) -> summary_type_2d {
        return {
          get<0>(accum),
          get<1>(accum) + next[0], 
          get<2>(accum) + next[1],
          get<3>(accum) + next[0] * next[0], 
          get<4>(accum) + next[0] * next[1], 
          get<5>(accum) + next[1] * next[1], 
        };});
  }

  template<typename forward_iterator>
  summary_type variance(
    const forward_iterator& begin, const forward_iterator& end
  ) {
    using namespace std;
    auto sums = sample_sums(begin, end);
    double size = get<0>(sums);
    double scale = 1. / (get<0>(sums) - 1.);
    return {
      size,
      get<1>(sums) / size,
      scale * (get<2>(sums) - get<1>(sums) * get<1>(sums) / size), 
    };
  }
  
  /**
   * 2-dim second-order summary - xbar, ybar, varx, covxy, vary
   */
  template<typename forward_iterator>
  summary_type_2d covariance(
    const forward_iterator& begin, const forward_iterator& end
  ) {
    using namespace std;
    auto sums = sample_sums_2d(begin, end);
    double size = get<0>(sums);
    double scale = 1. / (get<0>(sums) - 1.);

    return {
      size,
      get<1>(sums) / size,
      get<2>(sums) / size,
      scale * (get<3>(sums) - get<1>(sums) * get<1>(sums) / size), 
      scale * (get<4>(sums) - get<1>(sums) * get<2>(sums) / size), 
      scale * (get<5>(sums) - get<2>(sums) * get<2>(sums) / size)};
  }

  /**
   * control variate - returns mean and variance.
   * input is a container of std::pairs that have a forward iterator.
   */
  template<typename forward_iterator>
  summary_type summary(
    const forward_iterator& begin, 
    const forward_iterator& end, 
    double control_mean 
  ) {
    using namespace std;
    forward_iterator ptr = begin;
    auto cov = covariance(ptr, end);
    double corr = get<4>(cov) / (sqrt(get<3>(cov) * get<5>(cov)));

    std::vector<double> control_sample = {};
    transform(begin, end, back_inserter(control_sample), 
        [corr, control_mean](auto item) {
          return get<0>(item) - corr * (get<1>(item) - control_mean);});
    
    return variance(control_sample.begin(), control_sample.end());
  }
  
  enum hist_type {INCREMENTAL, CUMULATIVE, QUANTILE};

  struct histogram_request {
    const double accuracy_;
    const double quantum_;
    const hist_type type_;

    histogram_request(
        const histogram_request& request
    ): accuracy_(request.accuracy_),
       quantum_(request.quantum_),
       type_(request.type_) {}

    histogram_request(
        const double accuracy,
        const double quantum,
        const hist_type type
    ): accuracy_(accuracy),
       quantum_(quantum),
       type_(type) {}
  };

  struct histogram_datum {
    const size_t count_;
    const double quantile_;

    histogram_datum(): count_(0u), quantile_(0.) {}

    histogram_datum(
      const histogram_datum& datum
    ): count_(datum.count_),
       quantile_(datum.quantile_) {}

    histogram_datum(
      size_t count,
      double quantile
    ): count_(count),
      quantile_(quantile) {}
  };

  typedef std::tuple<double, size_t, std::map<int, size_t>> histogram_sketch;
  typedef std::tuple<hist_type, double, std::vector<histogram_datum>> histogram;

  template<
    typename container_type
  >
  histogram_sketch
  build_histogram_sketch (
    const histogram_request& request,
    const container_type sample
  ) {
    double gamma = (1. + request.accuracy_) / (1. - request.accuracy_);
    auto log_gamma = vol::utility::build_log(gamma);

    std::map<int, size_t> result;
    size_t total= 0;

    for (auto item: sample) {
      int index = static_cast<int>(std::floor(log_gamma(item)) + 0.5);
      if (result.find(index) == result.end()) {
        result[index] = 1.;
      } else {
        ++result[index];
      }
      ++total;
    }
    return histogram_sketch(request.accuracy_, total, result);
  }

  inline histogram
  build_histogram(
    const histogram_request& request,
    const histogram_sketch& sketch
  ) {
    double total = std::get<1>(sketch);
    double increment = request.quantum_ * total;
    double current_increment = increment;
    double current_gamma = (1. + std::get<0>(sketch)) / (1. - std::get<0>(sketch));
    double gamma = current_gamma;
    auto sketch_data = std::get<2>(sketch);
    std::vector<histogram_datum> result(static_cast<size_t>(1. / increment + 0.5));

    size_t count = 0u;
    for(auto item: sketch_data) {
      if (count + item.second >= current_increment) {
        result.emplace_back(count, current_increment / total);
        current_increment += increment;
        count += item.second;
        while(count >= current_increment) {
          result.emplace_back(count, current_increment / total);
          current_increment += increment;
        }
      } else {
        count += item.second;
      }
    }

    result.emplace_back(count, current_increment / total);
    for (auto item: result) {
      std::cout << "q: " << item.quantile_ << ", c: " << item.count_ << std::endl;
    }

    return histogram(request.type_, request.accuracy_, result);
  }
}
