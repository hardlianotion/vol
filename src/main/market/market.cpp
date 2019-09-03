#include "stats.hpp"
#include "market.h"

namespace vol {
  namespace market::vanilla {
   
    using namespace stats;

    double black(option o, double r, double f, double t, double v, double k) {
      double d1 = (log(f/k) + v*v*t / 2.)/(v*sqrt(t));
      double d2 = d1 - v*sqrt(t);
      switch (o) {
        case option::CALL:
          return exp(-r*t)*(f*pnorm(d1, 0., 1.) - k*pnorm(d2, 0., 1.));
          return exp(-r*t)*(f*pnorm(d1, 0., 1.) - k*pnorm(d2, 0., 1.));
          break;
        case option::PUT:
          return exp(-r*t)*(k*pnorm(-d2, 0., 1.) - f*pnorm(-d1, 0., 1.));
          return exp(-r*t)*(k*pnorm(-d2, 0., 1.) - f*pnorm(-d1, 0., 1.));
          break;
        default:
          std::cout << "NAN CALC" << std::endl;
          return std::numeric_limits<double>::quiet_NaN();
      }
    }

    double delta(option o, double r, double f, double t, double v, double k) {
      double d1 = (log(f/k) + v*v*t / 2.)/(v*sqrt(t));
      switch(o) {
        case option::CALL:
          return exp(-r*t)*stats::pnorm(d1, 0., 1.);
          break;
        case option::PUT:
          return exp(-r*t)*stats::pnorm(d1, 0., 1.) - 1.;
          break;
        default:
          return std::numeric_limits<double>::quiet_NaN();
      }
    }

    double gamma(option o, double r, double f, double t, double v, double k) {
      return delta(option::CALL, r, f, t, v, k)/(f*v*sqrt(t));
    }

    double vega(option, double r, double f, double t, double v, double k) {
      return f*delta(option::CALL, r, f, t, v, k)*sqrt(t);
    }

    double theta(option o, double r, double f, double t, double v, double k) {
      double d1 = (log(f/k) + v*v*t / 2.)/(v*sqrt(t));
      double d2 = d1 - v*sqrt(t);
      switch(o) {
        case option::CALL:
          return -(r + v/(2.*sqrt(t)))*f*delta(o, r, f, t, v, k) 
            + r*k*exp(-r*t)*pnorm(d2, 0., 1.);
          break;
        case option::PUT:
          return -v*f*delta(o, r, f, t, v, k)/(2*sqrt(t)) 
            - r*f*exp(-r*t)*pnorm(-d1, 0., 1.) + r*k*exp(-r*t)*pnorm(-d2, 0., 1.);  
          break;
        default:
          return std::numeric_limits<double>::quiet_NaN();
      }
    }

    double rho(option o, double r, double f, double t, double v, double k) {
      return -t*black(o, r, f, t, v, k);
    }
  }
  
  namespace market::asian {
    /**
     * geometric asian via black model
     */
    double geomAsian(
      option o, double r, double f, double t, double v, double k, double dt
    ) {
      double count = t / dt;
      double vbar = v * sqrt((2. * count + 1.)/(6. * (count + 1.)));
      double rho = 0.5 * (r - v*v / 2. + vbar * vbar);
      return vanilla::black(o, rho, f, t, vbar, k);
    }
  }
}

