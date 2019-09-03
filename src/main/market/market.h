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
     * this is the price of a geometric asian option.  
     * 
     */
    double geomAsian(
      option o, double r, double f, double t, double v, double k, double dt
    );
    
    /**
     * take a process and average the output between the time
     * start and end.
     */
    template<typename process>
    auto asianing(process p, double start, double end, double dt) {
      /*FIXME - hack.  can't figure out how to get iota 
      * and concepts to work
      * as desired.
      */ 
      if(start >= end) {
        std::ostringstream err;
        err << "start (" << start << ") must be less than (" << end <<")" 
            << std::endl;
        throw std::invalid_argument(err.str());
      }

      return [start, end, dt, p](double t) mutable {
        if(t < start)
          return 0.0;
        interval_d period(iterator(start, dt), iterator(std::min(t, end), dt));
        //FIXME - if this is 0, our tests fail.  If this is 1 they pass.  Why?
#if 0 
        std::cout << "p1: " << p(t) << std::endl; 
#endif  
        double result = 0.0;
        for (auto ptr: period) {
          result += p(ptr);
        }
        return result / utility::distance(period.begin(), period.end());};
//        return std::accumulate(period.begin(), period.end(), 0., 
//            [t, p](double agg, double inc) mutable -> double {
//            return agg + p(inc);
//            }) / utility::distance(period.begin(), period.end());};
    }
  }
}

