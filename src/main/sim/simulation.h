#pragma once 

#include <random>
#include <tuple>

#include <range/v3/all.hpp>


namespace vol {  
  
  namespace generator {
    
    template<typename gen_type, typename dist_type>
    auto create(dist_type&& adapter, std::seed_seq& seeds) {
      gen_type gen;
      gen.seed(seeds);
      return [adapter, gen]() mutable { return adapter(gen);};
    }

    template<typename gen_type, typename dist_type>
    auto create(dist_type&& adapter) {
      std::random_device rd{};
      gen_type gen(rd());
      return [adapter, gen]() mutable { return adapter(gen);};
    }

    // generators
    inline auto normal(double mu, double sigma) {
      typedef std::normal_distribution<>  norm;
      return create<std::mt19937_64, norm>(norm(mu, sigma));
    }
    
    inline auto normal(double mu, double sigma, std::seed_seq& seeds) {
      typedef std::normal_distribution<>  norm;
      return create<std::mt19937_64, norm>(norm(mu, sigma), seeds);
    }
    
    inline auto lognormal(double nu, double sigma) {
      typedef std::lognormal_distribution<> lognorm;
      return create<std::mt19937_64, lognorm>(lognorm(nu, sigma));
    }

    inline auto lognormal(double mu, double sigma, std::seed_seq& seeds) {
      typedef std::lognormal_distribution<> lognorm;
      return create<std::mt19937_64, lognorm>(lognorm(mu, sigma), seeds);
    }
    
    //test generator
    inline auto constant(double level) {
      return [level]() {return level;};
    }
  };

  namespace stats {

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
            get<1>(accum) + get<0>(next), 
            get<2>(accum) + get<1>(next),
            get<3>(accum) + get<0>(next)*get<0>(next), 
            get<4>(accum) + get<0>(next)*get<1>(next), 
            get<5>(accum) + get<1>(next)*get<1>(next), 
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
      double control_mean, 
      size_t size
    ) {
      using namespace std;
      forward_iterator ptr = begin;
      auto cov = covariance(ptr, end);
      double corr = get<4>(cov) / get<5>(cov);

      std::vector<double> control_sample = {};
      transform(begin, end, back_inserter(control_sample), 
          [corr, control_mean](auto item) {
            return get<0>(item) - corr * (get<1>(item) - control_mean);});
      
      return variance(control_sample.begin(), control_sample.end());
    }
  }
}
