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

  namespace proc {
    /**
     * white noise generator.
     **/
    inline auto bm() {
      auto impl = generator::normal(0., 1.);
      return [impl](double t) mutable {return sqrt(t) * impl();};
    }
 
    inline auto norm(double mu, double sigma) {
      auto impl = generator::normal(0., 1.);
      return [mu, sigma, impl](double t) mutable {
        return (sigma * sqrt(t) + mu) * impl();};
    } 

    //FIXME - study composition viz.
    //https://yongweiwu.wordpress.com/2015/01/03/generic-lambdas-and-the-compose-function/
    inline auto lognorm(double mu, double sigma) {
      auto impl = norm(mu, sigma);
      return [impl](double t) mutable {
        return exp(impl(t));};
    } 

    /**
     * this generator might be useful in formal testing.
     */
    inline auto constant(double level) {
      return [level](double t) {return level;};
    }
    /**
     * defines a procress defined by 
     * map \in map_type, gen \in gen_type
     * S(t) = map(t, gen(t))
     */
    template<
      typename gen_type,
      typename map_type
    >
    auto process(const map_type& map, const gen_type& gen) {
      return [map, gen](double t) {return map(t,gen(t)); }; 
    }

    /**
     * euler provides an euler update step for the SDE
     * dS(t) = drift(S(t),t) dt + vol(S(t),t) dW(t), 
     * S(0) = S0
     */
    template<
      typename vol_type,
      typename drift_type,
      typename gen_type
    >
    auto euler(
      const vol_type& vol, 
      const drift_type& drift, 
      gen_type& gen, 
      double dt
    ) {
      return [vol, drift, gen, dt]
        (double s, double t) mutable -> std::tuple<double, double> {return {s + vol(s, t) * gen(dt) + drift(s, t) * dt, t + dt};};
    }

    inline auto cir(double lambda, double mu, double sigma, double dt) {
      auto drift = [lambda, mu](double s, double) {return lambda*(mu - s);};
      auto vol = [sigma](double s, double) {return sigma*sqrt(s);};
      auto gen = bm();
      return euler(vol, drift, gen, dt);
    }

    inline auto ou(double lambda, double mu, double sigma, double dt) {
      auto drift = [lambda, mu](double s, double) {return lambda*(mu - s);};
      auto vol = [sigma](double s, double) {return sigma;};
      auto gen = bm();
      return euler(vol, drift, gen, dt);
    }
  }

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
            get<1>(accum) + next.first, 
            get<2>(accum) + next.second,
            get<3>(accum) + next.first*next.first, 
            get<4>(accum) + next.first*next.second, 
            get<5>(accum) + next.second*next.second, 
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
      double corr = get<3>(cov) / get<4>(cov);
      
      auto control_sample = ranges::views::iota(begin, end)
        | ranges::views::transform([corr, control_mean](auto item) {
            return item->first - corr * (item->second - control_mean);});
      
      return variance(control_sample.begin(), control_sample.end());
    }
  }
}
