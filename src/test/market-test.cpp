#define CATCH_CONFIG_MAIN 
#include "catch2/catch.hpp"

#include "market/market.h"
#include "sim/simulation.h"

SCENARIO ("Option contracts price respect invariants.", "[market]") {
  using namespace vol::market::vanilla;
  using namespace vol;
  
  double k = 100.;
  double itmf = 105.;
  double otmf = 97.;
  double r = 0.02;
  double t = 0.4;
  double v = 0.3;

  WHEN("Put and call option are live,") {
    double put_v = black(option::PUT, r, itmf, t, v, k);
    double call_v = black(option::CALL, r, itmf, t, v, k);
    double rho_v = rho(option::CALL, r, itmf, t, v, k);
    double gamma_v = gamma(option::CALL, r, itmf, t, v, k);
    double theta_v = theta(option::CALL, r, itmf, t, v, k);
    double delta_v = delta(option::CALL, r, itmf, t, v, k);

    THEN("Prices and risks are non-zero") {
      REQUIRE( put_v > 0. );
      REQUIRE( call_v > 0. );
      REQUIRE( theta_v < 0. );
      REQUIRE( gamma_v > 0. );
      REQUIRE( rho_v < 0. );
      REQUIRE( delta_v > 0. );
    }

    double fwd_v = exp(-r*t)*(itmf - k);    
    
    THEN("call - put is price of forward contract") {
      REQUIRE_THAT( call_v - put_v, Catch::WithinAbs(fwd_v, 1.e-8) );
    }

    THEN("The greeks are relateve via the pde") {
      REQUIRE_THAT( theta_v + v*v*itmf*itmf*gamma_v/2., Catch::WithinAbs(r/t*rho_v, 1.e-8) );
    }
  }

  WHEN("asianing is carried out on an underlying process") {
    using vol::proc::constant;
    using vol::proc::linear;
    double t = 10.;
    double dt = 1.;
    auto asianConst = market::asian::asianing(constant(1.), 1., t, dt);
    
    THEN("the output is the sum of the process samples over the period") {
      REQUIRE( asianConst(t) == 1. );
    }
    
    auto asianLin = market::asian::asianing(linear(1., 1.), 1., t, dt);
    
    THEN("the output is the sum of the process samples over the period") {
      REQUIRE( asianLin(t) == 6. );
      REQUIRE( asianLin(t - 1.) == 5.5 );
    }

    THEN("Underlying process samples are not contributed past expiry.") {
      REQUIRE( asianLin(20.) == 6. );
    }
  }
}

SCENARIO ("Market contracts are driven by stochastic processes..", "[market]") {
  WHEN("asianing is carried out using a stochastic process") {
    using vol::proc::constant;
    using vol::proc::linear;
    using vol::proc::norm;
    using vol::proc::lognorm;
    double t = 10.;
    double dt = 1.;

    auto lin_d = linear(1., 1.);
    auto norm_d = norm(0.1, 0.4);
    auto lognorm_d = lognorm(0.1, 0.4);
    
    auto asianLin = vol::market::asian::asianing(linear(1., 1.), 0., t, dt);
    auto asianNorm = vol::market::asian::asianing(norm(0.1, 0.4), 0., t, dt);
    auto asianLogNorm = vol::market::asian::asianing(lognorm(0.1, 0.4), 0., t, dt);
    
    THEN("successive draws from the underlying process are different.") {
      CHECK( t != 2. * t );
      CHECK( norm_d(t) != norm_d(t) );
      CHECK( norm_d(2. * t) != norm_d(t) );
      CHECK( lognorm_d(t) != lognorm_d(t) );
      CHECK( lognorm_d(2. * t) != lognorm_d(t) );
    }
    
    THEN("successive draws from the asianed process are different.") {
      CHECK( t != 2. * t );
      CHECK( asianNorm(t) != asianNorm(0.5*t) );
      CHECK( asianNorm(2. * t) != asianNorm(t) );
      CHECK( asianLogNorm(t) != asianLogNorm(t) );
      CHECK( asianLogNorm(2. * t) != asianLogNorm(t) );
    }
  }
}
