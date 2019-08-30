#pragma once 

#include <random>

namespace vol {  
  
  struct Generator {
    
    template<typename gen_type, typename dist_type>
    static auto create(dist_type&& adapter) {
      std::random_device rd{};
      gen_type gen(rd());
      return [adapter, gen]() mutable { return adapter(gen);};
    }

    // generators
    static auto normal(double mu, double sigma) {
      typedef std::normal_distribution<>  norm;
      return create<std::mt19937_64, norm>(norm(mu, sigma));
    }
    
    static auto lognormal(double nu, double sigma) {
      typedef std::lognormal_distribution<> lognorm;
      return create<std::mt19937_64, lognorm>(lognorm(nu, sigma));
    }

    //test generator
    static auto constant(const double level) {
      return [level]() {return level;};
    }
  };



}

