#pragma once 
#include "utility.h"
/**
 * FIXME - this class is here until I can figure out
 * what structure in the Nieber's range library gives
 * me desired behaviour.
 *
 * At the moment, I can't make view::iota work for me.  Usage 
 * requires another transformation and the structure
 * seems to be lazy and caching when I use it.
 */

namespace vol::utility {
  template<typename T>
  struct titerator;

  template<typename T>
  double distance(const titerator<T>& lhs, const titerator<T>& rhs); 

  template<typename T>
  bool operator == (const T& lhs, const titerator<T>& rhs);

  template<typename T>
  bool operator == (const titerator<T>& rhs, const titerator<T>& lhs);

  template<typename T>
  bool operator == (const titerator<T>& lhs, const T& rhs);

  template<typename T>
  bool operator != (const T& lhs, const titerator<T>& rhs); 

  template<typename T>
  bool operator != (const titerator<T>& lhs, const titerator<T>& rhs);
  
  template<typename T>
  bool operator != (const titerator<T>& lhs, const T& rhs);
  
  template<typename T>
  bool operator < (const T& lhs, const titerator<T>& rhs);
  
  template<typename T>
  bool operator < (const titerator<T>& lhs, const titerator<T>& rhs);
  
  template<typename T>
  bool operator < (const titerator<T>& lhs, const T& rhs);
  
  template<typename T>
  bool operator <= (const T& lhs, const titerator<T>& rhs);
  
  template<typename T>
  bool operator <= (const titerator<T>& lhs, const titerator<T>& rhs);
  
  template<typename T>
  bool operator <= (const titerator<T>& lhs, const T& rhs);

  template<typename T>
  struct titerator {
    
    using iterator_category = std::forward_iterator_tag;
    using value_type = double;
    using difference_type = double;
    using pointer = double*;
    using reference = double&;

    titerator(
      const T& loc,
      const T& inc
    ): inc_(inc), loc_(loc) {}

    titerator(
      const titerator& other
    ): inc_(other.inc_), loc_(other.loc_) {}
    
    titerator& operator++() {
      loc_ += inc_;
      return *this;
    }

    titerator&& operator++(int) const {
      titerator result(*this);
      ++(*this);
      return std::move(*result);
    }

    titerator& operator--() {
      loc_ -= inc_;
      return *this;
    }

    T& operator*() {
      return loc_;
    }

    const T& operator*() const {
      return loc_;
    }

    titerator&& operator--(int) const {
      titerator result(*this);
      --(*this);
      return std::move(*result);
    }

    titerator& operator=(const titerator& rhs) {
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
  inline const titerator<T>& min (
    const titerator<T>& begin, const titerator<T>& end
  ) {
    return begin.loc_ < end.loc_ ? begin : end;
  }

  template<typename T>
  inline const titerator<T>& max (
    const titerator<T>& begin, const titerator<T>& end
  ) {
    return begin.loc_ < end.loc_ ? end : begin;
  }

  template<typename T>
  struct interval {
    typedef titerator<T> iterator;
    using iterator_category = typename iterator::iterator_category;
    using value_type = typename iterator::value_type;
    using difference_type = typename iterator::difference_type;
    using pointer = typename iterator::pointer;
    using reference = typename iterator::reference;

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
    const titerator<T>& lhs, 
    const titerator<T>& rhs
  ) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return (rhs.loc_ - lhs.loc_) / lhs.inc_;
  }

  template<typename T>
  inline bool operator == (const T& lhs, const titerator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ == rhs.loc_;
  }

  template<typename T>
  inline bool operator == (
    const titerator<T>& rhs, 
    const titerator<T>& lhs
  ) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ == rhs.loc_;
  }

  template<typename T>
  inline bool operator == (const titerator<T>& lhs, const T& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ == rhs.loc_;
  }

  template<typename T>
  inline bool operator != (const T& lhs, const titerator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ != rhs.loc_;
  }

  template<typename T>
  inline bool operator != (const titerator<T>& lhs, const titerator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ != rhs.loc_;
  }
  
  template<typename T>
  inline bool operator != (const titerator<T>& lhs, const T& rhs) {
    return lhs.loc_ != rhs.loc_;
  }
  
  template<typename T>
  inline bool operator < (const T& lhs, const titerator<T>& rhs) {
    return lhs.loc_ < rhs.loc_;
  }
  
  template<typename T>
  inline bool operator < (const titerator<T>& lhs, const titerator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ <  rhs.loc_;
  }
  
  template<typename T>
  inline bool operator < (
    const titerator<T>& lhs, 
    const T& rhs
  ) {
    return lhs.loc_ < rhs.loc_;
  }
  
  template<typename T>
  inline bool operator <= (const T& lhs, const titerator<T>& rhs) {
    return lhs.loc_ <= rhs.loc_;
  }
  
  template<typename T>
  inline bool operator <= (const titerator<T>& lhs, const titerator<T>& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ <= rhs.loc_;
  }
  
  template<typename T>
  inline bool operator <= (const titerator<T>& lhs, const T& rhs) {
    return lhs.loc_ <= rhs.loc_;
  }
}
