#pragma once


#include <cmath>

namespace vol::utility {
  template <typename T> int sgn(T val) {
     return (T(0) < val) - (val < T(0));
  }

  /**
   * We provide this as a lambda as we don't want to recalculate log base each call.
   */
  inline auto build_log(double base) {
    double inv_log_base = 1. / std::log(base);
    return [inv_log_base] (double x) -> double {
      return std::log(x) * inv_log_base;
    };
  }

  inline auto build_exp(double base) {
    double log_base = std::log(base);
    return [log_base] (double x) -> double {
      return std::log(x) * log_base;
    };
  }
  
  /**
   * intended to apply a collection of operators against data a.
   *
   * FIXME - this is a rough idea that needs to be tested for usability.
   */
  template<typename iterator, typename back_iter, typename T>
  void transform(iterator begin, iterator end, back_iter back_ptr, T data) {
    for (iterator iter = begin; iter != end; ++iter) {
      *(back_ptr) = (*iter)(data);
      ++back_ptr;
    }
  }
}

