#pragma once 
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
  struct interval {
    typedef iterator<T> iterator;
    typedef int ptr_diff;
  
    interval(
      iterator&& begin, iterator&& end
    ): bounds_(std::make_pair(std::min(begin, end),std::max(begin, end))) {}
    
    const iterator& begin() const {
      return bounds_.first;
    } 

    const iterator& end() const {
      return bounds_.second;
    } 

    const std::pair<iterator, iterator> bounds_;
  };

  template<typename T>
  inline int distance(
    const typename interval<T>::iterator& lhs, 
    const typename interval<T>::iterator& rhs
  ) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return static_cast<int>((rhs.loc_ - lhs.loc_) / lhs.inc_);
  }

  template<typename T>
  inline bool operator == (const T& lhs, const typename interval<T>::iterator& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ == rhs.loc_;
  }

  template<typename T>
  inline bool operator == (
    const typename interval<T>::iterator& rhs, 
    const typename interval<T>::iterator& lhs
  ) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ == rhs.loc_;
  }

  template<typename T>
  inline bool operator == (const typename interval<T>::iterator& lhs, const T& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ == rhs.loc_;
  }

  template<typename T>
  inline bool operator != (const T& lhs, const typename interval<T>::iterator& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ != rhs.loc_;
  }

  template<typename T>
  inline bool operator != (const typename interval<T>::iterator& lhs, const typename interval<T>::iterator& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ != rhs.loc_;
  }
  
  template<typename T>
  inline bool operator != (const typename interval<T>::iterator& lhs, const T& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ != rhs.loc_;
  }
  
  template<typename T>
  inline bool operator < (const T& lhs, const typename interval<T>::iterator& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ < rhs.loc_;
  }
  
  template<typename T>
  inline bool operator < (
    const iterator<T>& lhs, 
    const iterator<T>& rhs 
  ) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ <  rhs.loc_;
  }
  
  template<typename T>
  inline bool operator < (
    const iterator<T>& lhs, 
    const T& rhs
  ) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ < rhs.loc_;
  }
  
  template<typename T>
  inline bool operator <= (const T& lhs, const typename interval<T>::iterator& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ <= rhs.loc_;
  }
  
  template<typename T>
  inline bool operator <= (const typename interval<T>::iterator& lhs, const typename interval<T>::iterator& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();

    return lhs.loc_ <= rhs.loc_;
  }
  
  template<typename T>
  inline bool operator <= (const typename interval<T>::iterator& lhs, const T& rhs) {
    if(lhs.inc_ != rhs.inc_)
      return std::numeric_limits<int>::signaling_NaN();
    
    return lhs.loc_ <= rhs.loc_;
  }
}
