#pragma once


namespace vol::utility {
  template <typename T> int sgn(T val) {
     return (T(0) < val) - (val < T(0));
  }
  
  /**
   * intended to apply a collection of operators against data a.
   *
   * FIXME - this is a rough idea that needs to be tested for usability.
   */
  template<typename iterator, typename back_iter, typename T>
  void transform(iterator begin, iterator end, back_iter back_ptr, T data) {
    for (iterator iter = begin; iter != end; ++iter) {
      auto test = (*iter)(data);
      *(back_ptr) = test;
      ++back_ptr;
    }
  }
}

