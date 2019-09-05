#pragma once 
#include <functional>


/**
 * This implementation comes from Wu Yongwei.
 *
 * Wu Yongwei - https://yongweiwu.wordpress.com/2014/12/07/study-notes-functional-programming-with-cplusplus/comment-page-1/#comment-14257
*/
namespace vol::utility {
  
  inline auto build_identity() {return [](auto x) -> decltype(x) {return x;};}

  inline auto compose () {
    return [](auto&& x) -> decltype(auto)
    {
      return std::forward<decltype(x)>(x);
    };
  }

  template <typename fn_type>
  auto compose (fn_type&& fn) {
    return [fn=std::forward<decltype(fn)>(fn)](auto&&... x) mutable  -> decltype(auto) {
      return fn(std::forward<decltype(x)>(x)...);
    };
  }
   
  template <typename fn_type, typename... fn_types>
  auto compose (fn_type fn, fn_types... args) {
    return [fn=std::forward<decltype(fn)>(fn),
            args...](auto&&... x) mutable -> decltype(auto) {
      return fn(
        compose(args...)(std::forward<decltype(x)>(x)...));
    };
  }

  template<
    typename D, 
    typename R, 
    size_t N, 
    typename fn_type>
  void agg_impl(
    D x, 
    std::array<R, N>& result, 
    fn_type fn
  ) {
    result[N - 1] = fn(x);
  }

  template<
    typename D, 
    typename R, 
    size_t N, 
    typename fn_type,
    typename... fn_types> 
  void agg_impl(
    D x, 
    std::array<R, N>& result, 
    fn_type fn,
    fn_types... fns
  ) {
    static_assert(N > (sizeof...(fn_types)));
    size_t i = N - 1 - sizeof...(fn_types);
    agg_impl(x, result, fns...);
    result[i] = fn(x);
  }

  template <typename fn_type, typename... fn_types>
  auto aggregate(fn_type fn, fn_types... fns) {
    return [&fn, fns...](auto x) mutable
      -> std::array<decltype(fn(x)), sizeof...(fns) + 1> {
      std::array<decltype(fn(x)), sizeof...(fns)+ 1> result;
      agg_impl(x, result, fn, fns...);
      return result;
    };
  }
  
  template<typename payoff_type, typename container_type>
  auto replicate(payoff_type fn, container_type) {
    return [&fn](container_type&& x) mutable -> container_type {
      container_type result = x;
      std::transform(result.begin(), result.end(), result.begin(), fn);
      return result;
    };
  }
}

