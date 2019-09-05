#pragma once
#include <sstream>
#include "simulation.h"


namespace vol::proc {
  /**
   * white noise generator.
   */
  inline auto bm() {
    auto impl = generator::normal(0., 1.);
    return [impl](double t) mutable {return sqrt(t) * impl();};
  }
 
  inline auto norm(double mu, double sigma) {
    auto impl = generator::normal(0., 1.);
    return [mu, sigma, impl](double t) mutable {
        auto z = impl();
      return (sigma * sqrt(t) + mu) * z;};
  } 

  //FIXME - study composition viz.
  //https://yongweiwu.wordpress.com/2015/01/03/generic-lambdas-and-the-compose-function/
  inline auto lognorm(double initial, double mu, double sigma) {
    auto impl = norm(mu, sigma);
    return [initial, impl](double t) mutable {
      return initial * exp(impl(t));};
  } 

  /**
   * this generator might be useful in formal testing.
   */
  inline auto constant(double level) {
    return [level](double t) -> double  { return level; };
  }

  inline auto linear(double scale, double level) {
    return [scale, level](double t) -> double { return t * scale + level; };
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
   * Given a process, run it over a time period and return its output
   */

template <
  typename proc_type,
  typename container_type
  >
  auto run_over(proc_type proc, double begin, double end, double dt) {
    if(begin >= end) {  
      std::ostringstream err;
      err << "begin (" << begin << ") must be less than (" << end <<")" 
          << std::endl;
      throw std::invalid_argument(err.str());
    }

    return [proc, begin, end, dt] (double t) mutable -> container_type {
      container_type result;

      for (double s = begin; s < std::min(t, end); s += dt) {
        result.push_back(std::make_pair(s, proc(s)));
      }
      return result;
    };
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

  //create n samples from a
  template<typename proc_type, typename container_type>
  auto sample(proc_type& proc, size_t n) -> container_type {
    container_type result;
    for(size_t i = 0; i < n; ++i) {
      result.push_back(proc());
    }
    return result;
  }
}

