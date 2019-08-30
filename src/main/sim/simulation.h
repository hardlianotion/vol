#pragma once 


namespace vol {
  
  
  struct Simulation {
    // build normal generator
    static auto normal(const double mu, const double sigma) {
      return []() {};
    }
  };
}

