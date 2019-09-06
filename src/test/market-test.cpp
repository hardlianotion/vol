#define CATCH_CONFIG_MAIN 
#include "catch2/catch.hpp"

#include "market/market.h"
#include "market/buildmarket.h"
#include "sim/buildprocess.h"
#include "sim/process.h"
#include "sim/simulation.h"
#include "utility/lambda.h"


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

    THEN("The greeks are related via the pde") {
      CHECK_THAT( theta_v + v*v*itmf*itmf*gamma_v/2., Catch::WithinAbs(r/t*rho_v, 1.e-8) );
    }
  }

  WHEN("asianing is carried out on an underlying path") {
    using namespace vol::proc;
    using namespace vol::market;

    double begin = 1.;
    double end = 10.;
    double longPathEnd= 20.;
    double shortPathEnd= 9.;
    double dt = 1.;

    path_type constPath = buildConstPath(1., begin, end, dt);

    auto asian = buildAsian(begin, end, dt);

    THEN("the output is the sum of he process samples over period [start, end)") {
      CHECK_THAT( asian(constPath), Catch::WithinAbs(1., 1.e-6) );
    }
    double level = 1.;
    double scale = 1.;
    path_type linPath = buildLinearPath(level, scale, begin, end, dt);
    path_type longLinPath = buildLinearPath(level, scale, begin, longPathEnd, dt);
    path_type shortLinPath = buildLinearPath(level, scale, begin, shortPathEnd, dt);

    THEN("the output is the average of the process samples over the period") {
      CHECK_THAT( asian(linPath), Catch::WithinAbs(6.0, 1.e-6));
      CHECK_THAT( asian(shortLinPath), Catch::WithinAbs(4.888889, 1e-5 ));
    }

    THEN("Underlying process samples are not contributed past expiry.") {
      CHECK_THAT( asian(longLinPath), Catch::WithinAbs(6.0, 1.e-6));
    }
  }
}

SCENARIO ("Market contracts are driven by stochastic processes..", "[market]") {
  WHEN("asianing is carried out using a stochastic process") {
    using namespace vol;
    using namespace vol::market;
    using namespace vol::proc;

    double begin = 0.;
    double end = 10.;
    double dt = 1.;

    double level = 1.;
    double strike = 1.;
    double mu = 0.1;
    double vol= 0.4;

    auto asian = buildAsian(begin, end, dt);

    auto normPath = buildNormPath(mu, vol, begin, end, dt);
    auto logNormPath = buildLogNormalPath(level, mu, vol, begin, end, dt);

    std::vector<pt_type> normPath1 = normPath(end), normPath2 = normPath(end);
    std::vector<pt_type> logNorm1 = logNormPath(end), logNormPath2 = logNormPath(end);

    for(size_t i = 0; i < normPath1.size(); ++i) {
      CHECK_THAT(normPath1[i].second, !Catch::WithinAbs(normPath2[i].second, 1.e-6));
      CHECK_THAT(logNorm1[i].second, !Catch::WithinAbs(logNormPath2[i].second, 1.e-6));
    }
//    These won't work until I have fixed up a match adapter for std::array
//    THEN("successive paths drawn from stochastic processes are different.") {
//      std::vector<pt_type> normPath1 = normPath(end), normPath2 = normPath(end);
//      std::Vector<pt_type> logNorm1 = logNormPath(end), logNormPath2 = logNormPath(end);
//      CHECK_THAT(normPath1, Catch::Approx(normPath2).epsilon(1.e-6));
//      CHECK_THAT(logNorm1, Catch::Within(logNormPath2).epsilon(1.e-6));
//    }

    auto asianNorm = utility::compose(asian, normPath);
    auto asianLogNorm = utility::compose(asian, logNormPath);
    THEN("successive calculations from asianed processes are different.") {
      CHECK_THAT( asianNorm(end), !Catch::WithinAbs(asianNorm(end), 1.e-6));
      CHECK_THAT( asianLogNorm(end), !Catch::WithinAbs(asianLogNorm(end), 1.e-6));
    }

    auto payoff = vanilla::payoff(option::CALL, strike);
    auto asianNormCalls = vol::utility::compose(payoff, asianNorm);
    auto asianLogNormCalls = vol::utility::compose(payoff, asianNorm);
    THEN("successive calculations from vanilla processes are different.") {
      CHECK_THAT( asianNormCalls(end), !Catch::WithinAbs(asianNormCalls(end), 1.e-6));
      CHECK_THAT( asianLogNormCalls(end), !Catch::WithinAbs(asianLogNormCalls(end), 1.e-6));
    }
  }
}
