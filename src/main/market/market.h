#pragma once
#include <sstream>
#include <exception>
#include <memory>

#include "utility/utility.h"


namespace vol {
  enum class option {CALL, PUT};

  struct Option {
    explicit Option(option type): type_(type) {}
    option type_;
  };

  namespace market::vanilla {

    double black(option o, double r, double f, double t, double v, double k);

    double delta(option o, double r, double f, double t, double v, double k);

    double gamma(option o, double r, double f, double t, double v, double k);

    double vega(option o, double r, double f, double t, double v, double k);

    double theta(option o, double r, double f, double t, double v, double k);

    double rho(option o, double r, double f, double t, double v, double k);

    inline auto payoff(option o, double k) {
      auto impl = std::make_shared<option>(o);
      return [impl, k](double s) {
        switch(*impl) {
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
    /**
     * this is the analytic price of a geometric asian option.  
     * 
     */
    double geomAsian(
      option o, double r, double f, double t, double v, double k, double dt
    );

    /**
     * take a process and average the output between the time
     * start and end.  Transform each point with pt_fn before averaging
     * and the averaged result with path_fn.
     */
    template<typename path_type, typename pt_fn_type, typename path_fn_type>
    auto asianing(
      pt_fn_type pt_fn, 
      path_fn_type path_fn, 
      double begin, 
      double end,
      double dt
    ) {
      return [path_fn, pt_fn, begin, end, dt](const path_type& path) mutable {
      double result = 0.;

      for (auto pt: path) {
        if(pt.first >= begin && pt.first < end) 
          result += pt_fn(pt.second);
      }
      return path_fn(result / ((end - begin) / dt));};
    }
  }
}

