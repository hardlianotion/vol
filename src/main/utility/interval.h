#pragma once 
#include "utility.h"
/**
 * FIXME - this class is here until I can figure out
 * what structure in the Nieber's range library gives
 * me desired behaviour.
 *
 * At the moment, view::iota does not quite fit.  Usage 
 * requires another transformation and the structure
 * seems to be lazy and caching when I use it.
 */
namespace vol::utility {
  template<typename T>
  struct iterator;

  template<typename T>
  double distance(const iterator<T>& lhs, const iterator<T>& rhs); 

  template<typename T>
  bool operator == (const T& lhs, const iterator<T>& rhs);

  template<typename T>
  bool operator == (const iterator<T>& rhs, const iterator<T>& lhs);

  template<typename T>
  bool operator == (const iterator<T>& lhs, const T& rhs);

  template<typename T>
  bool operator != (const T& lhs, const iterator<T>& rhs); 

  template<typename T>
  bool operator != (const iterator<T>& lhs, const iterator<T>& rhs);
  
  template<typename T>
  bool operator != (const iterator<T>& lhs, const T& rhs);
  
  template<typename T>
  bool operator < (const T& lhs, const iterator<T>& rhs);
  
  template<typename T>
  bool operator < (const iterator<T>& lhs, const iterator<T>& rhs);
  
  template<typename T>
  bool operator < (const iterator<T>& lhs, const T& rhs);
  
  template<typename T>
  bool operator <= (const T& lhs, const iterator<T>& rhs);
  
  template<typename T>
  bool operator <= (const iterator<T>& lhs, const iterator<T>& rhs);
  
  template<typename T>
  bool operator <= (const iterator<T>& lhs, const T& rhs);

  template<typename T>
  struct iterator {
    iterator(
      const T& loc,
      const T& inc
    ): inc_(inc), loc_(loc) {}

    iterator(
      const iterator& other
    ): inc_(other.inc_), loc_(other.loc_) {}
    
    iterator& operator++() {
      loc_ += inc_;
      return *this;
    }

    iterator&& operator++(int) const {
      iterator result(*this);
      ++(*this);
      return std::move(*result);
    }

    iterator& operator--() {
      loc_ -= inc_;
      return *this;
    }

    T& operator*() {
      return loc_;
    }

    const T& operator*() const {
      return loc_;
    }

    iterator&& operator--(int) const {
      iterator result(*this);
      --(*this);
      return std::move(*result);
    }

    iterator& operator=(const iterator& rhs) {
      loc_ = rhs.loc_;
      return *this;
    }
    //FIXME - If I uncomment this and make the comparison operator friends of this class
    //I get a missing symbol link error.  Why is this?
//private 
    const T inc_;
    T loc_;
  };
  
  template<typename T>
  inline const iterator<T>& min (
    const iterator<T>& begin, const iterator<T>& end
  ) {
    return begin.loc_ < end.loc_ ? begin : end;
  }

  template<typename T>
  inline const iterator<T>& max (
    const iterator<T>& begin, const iterator<T>& end
  ) {
    return begin.loc_ < end.loc_ ? end : begin;
  }

  template<typename T>
  struct interval {
    typedef iterator<T> iterator;
    typedef int ptr_diff;
  
    interval(
      const iterator& begin, const iterator& end
    ): bounds_(
        std::make_pair(
          utility::min(begin, end),
          utility::max(begin, end))) {}
    
    const iterator& begin() const {
      return bounds_.first;
    } 

    const iterator& end() const {
      return bounds_.second;
    } 

    const std::pair<iterator, iterator> bounds_;
  };

  template<typename T>
  inline double distance(
    const iterator<T>& lhs, 
    const iterator<T>& rhs
  ) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return (rhs.loc_ - lhs.loc_) / lhs.inc_;
  }

  template<typename T>
  inline bool operator == (const T& lhs, const iterator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ == rhs.loc_;
  }

  template<typename T>
  inline bool operator == (
    const iterator<T>& rhs, 
    const iterator<T>& lhs
  ) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ == rhs.loc_;
  }

  template<typename T>
  inline bool operator == (const iterator<T>& lhs, const T& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ == rhs.loc_;
  }

  template<typename T>
  inline bool operator != (const T& lhs, const iterator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ != rhs.loc_;
  }

  template<typename T>
  inline bool operator != (const iterator<T>& lhs, const iterator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ != rhs.loc_;
  }
  
  template<typename T>
  inline bool operator != (const iterator<T>& lhs, const T& rhs) {
    return lhs.loc_ != rhs.loc_;
  }
  
  template<typename T>
  inline bool operator < (const T& lhs, const iterator<T>& rhs) {
    return lhs.loc_ < rhs.loc_;
  }
  
  template<typename T>
  inline bool operator < (const iterator<T>& lhs, const iterator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ <  rhs.loc_;
  }
  
  template<typename T>
  inline bool operator < (
    const iterator<T>& lhs, 
    const T& rhs
  ) {
    return lhs.loc_ < rhs.loc_;
  }
  
  template<typename T>
  inline bool operator <= (const T& lhs, const iterator<T>& rhs) {
    return lhs.loc_ <= rhs.loc_;
  }
  
  template<typename T>
  inline bool operator <= (const iterator<T>& lhs, const iterator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ <= rhs.loc_;
  }
  
  template<typename T>
  inline bool operator <= (const iterator<T>& lhs, const T& rhs) {
    return lhs.loc_ <= rhs.loc_;
  }
}
