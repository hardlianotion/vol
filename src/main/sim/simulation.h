#pragma once 

#include <random>

namespace vol {  
  
  struct Generator {
    // build normal generator
    static auto normal(const double mu, const double sigma) {
      std::random_device rd{};
      std::mt19937_64 gen(rd());
      auto norm = std::normal_distribution<>(mu, sigma);
      return [norm, gen]() mutable { double result = (norm)(gen); return result;};
    };

  };


}

