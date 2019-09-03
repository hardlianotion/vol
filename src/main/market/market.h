#pragma once
#include <iostream>
#include <sstream>
#include <exception>

#include "stats.hpp"
#include "utility/interval.h"


namespace vol {
  enum class option {CALL, PUT};
  namespace market::vanilla {

    double black(option o, double r, double f, double t, double v, double k);

    double delta(option o, double r, double f, double t, double v, double k);

    double gamma(option o, double r, double f, double t, double v, double k);

    double vega(option o, double r, double f, double t, double v, double k);

    double theta(option o, double r, double f, double t, double v, double k);

    double rho(option o, double r, double f, double t, double v, double k);

    inline auto payoff(option o, double k) {
      return [o, k](double s) {
        switch(o) {
          case option::CALL:
            return std::max(s - k, 0.);
            break;
          case option::PUT:
            return std::max(k - s, 0.);
            break;
          default:
            return std::numeric_limits<double>::quiet_NaN();
        }
      };
    }
  }

  namespace market::asian {

    typedef utility::iterator<double> iterator;
    typedef utility::interval<double> interval_d;
    /**
     * this is the analytic price of a geometric asian option.  
     * 
     */
    double geomAsian(
      option o, double r, double f, double t, double v, double k, double dt
    );
    /**
     *
     */
    template<typename iterator>
    double geomAsianing(double start, double end, double dt) {
      
    }

    /**
     * take a process and average the output between the time
     * start and end.
     */
    template<typename process>
    auto asianing(process p, double start, double end, double dt) {
      if(start >= end) {
        std::ostringstream err;
        err << "start (" << start << ") must be less than (" << end <<")" 
            << std::endl;
        throw std::invalid_argument(err.str());
      }

      return [start, end, dt, p](double t) mutable {
        double result = 0.;
        if(t < start) {
          return result;
        }

        for (double s = start; s < std::min(t, end); s += dt) {
          result += p(s);
        }
        return result / static_cast<size_t>((end - start) / dt + 0.5);};
    }
  }
}

