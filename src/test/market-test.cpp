#define CATCH_CONFIG_MAIN 
#include "catch2/catch.hpp"

#include "market/market.h"

SCENARIO ("Option contracts price respect invariants.", "[market]") {
  using namespace vol::market;
  
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
}
