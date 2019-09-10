#pragma once 

#include <random>
#include <tuple>


namespace vol::generator {  

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
    return [level]() { return level; };
  }
}
