#define CATCH_CONFIG_MAIN 
#include "catch2/catch.hpp"

#include "market/market.h"
#include "sim/process.h"
#include "sim/simulation.h"
#include "utility/lambda.h"
#include "market_test_objects.h"

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
      CHECK( put_v > 0. );
      CHECK( call_v > 0. );
      CHECK( theta_v < 0. );
      CHECK( gamma_v > 0. );
      CHECK( rho_v < 0. );
      CHECK( delta_v > 0. );
    }

    double fwd_v = exp(-r*t)*(itmf - k);    
    
    THEN("call - put is price of forward contract") {
      CHECK_THAT( call_v - put_v, Catch::WithinAbs(fwd_v, 1.e-8) );
    }

    THEN("The greeks are relateve via the pde") {
      CHECK_THAT( theta_v + v*v*itmf*itmf*gamma_v/2., Catch::WithinAbs(r/t*rho_v, 1.e-8) );
    }
  }

  WHEN("asianing is carried out on an underlying path") {
    using namespace vol::proc;
    using namespace test;

    double begin = 1.;
    double end = 10.;
    double longPathEnd= 20.;
    double shortPathEnd= 9.;
    double dt = 1.;
    
    path_type constPath = buildConstPath(1., begin, end, dt);

    auto asian = buildAsian(begin, end, dt);
    
    THEN("the output is the sum of he process samples over period [start, end)") {
      CHECK( asian(constPath) == 1. );
    }
    double level = 1.;
    double scale = 1.;
    path_type linPath = buildLinearPath(level, scale, begin, end, dt);
    path_type longLinPath = buildLinearPath(level, scale, begin, longPathEnd, dt);
    path_type shortLinPath = buildLinearPath(level, scale, begin, shortPathEnd, dt);
    
    THEN("the output is the average of the process samples over the period") {
      CHECK( asian(linPath) == 6.0);
      CHECK_THAT( asian(shortLinPath), Catch::WithinAbs(4.888889, 1e-5 ));
    }

    THEN("Underlying process samples are not contributed past expiry.") {
      CHECK( asian(longLinPath) == 6.0 );
    }
  }
}

SCENARIO ("Market contracts are driven by stochastic processes..", "[market]") {
  WHEN("asianing is carried out using a stochastic process") {
    using namespace test;
    using namespace vol::market;

    double begin = 0.;
    double end = 10.;
    double dt = 1.;

    double level = 1.;
    double mu = 0.1;
    double vol= 0.4;
    
    auto asian = buildAsian(begin, end, dt);
    
    THEN("successive paths drawn from stochastic processes are different.") {
      CHECK( asian(buildNormPath(mu, vol, begin, end, dt)) != asian(buildNormPath(mu, vol, begin, end, dt)));
      CHECK( asian(buildLogNormalPath(level, mu, vol, begin, end, dt)) != asian(buildLogNormalPath(level, mu, vol, begin, end, dt)));
    }
  }
}
