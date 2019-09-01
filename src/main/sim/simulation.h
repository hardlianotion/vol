#pragma once 

#include <random>
#include <tuple>


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
    auto normal(double mu, double sigma) {
      typedef std::normal_distribution<>  norm;
      return create<std::mt19937_64, norm>(norm(mu, sigma));
    }
    
    auto normal(double mu, double sigma, std::seed_seq& seeds) {
      typedef std::normal_distribution<>  norm;
      return create<std::mt19937_64, norm>(norm(mu, sigma), seeds);
    }
    
    auto lognormal(double nu, double sigma) {
      typedef std::lognormal_distribution<> lognorm;
      return create<std::mt19937_64, lognorm>(lognorm(nu, sigma));
    }

    auto lognormal(double mu, double sigma, std::seed_seq& seeds) {
      typedef std::lognormal_distribution<> lognorm;
      return create<std::mt19937_64, lognorm>(lognorm(mu, sigma), seeds);
    }
    
    //test generator
    auto constant(double level) {
      return [level]() {return level;};
    }
  };

  namespace proc {
    /**
     * white noise generator.
     **/
    auto brownian() {
      return [](double t) {return generator::normal(0.,sqrt(t));};
    }
    
    /**
     * this generator might be useful in formal testing.
     */
    auto constant(double level) {
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
      const gen_type& gen, 
      double dt
    ) {
      return [vol, drift, gen, dt]
        (double s, double t) {return std::make_tuple(s + vol(s, t) * gen(dt) + drift(s, t) * dt, t + dt);};
    }
  }
}
