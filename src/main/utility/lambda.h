#pragma once 
#include <functional>
#include <iostream>

/**
 * This implementation comes from Wu Yongwei.
 *
 * Wu Yongwei - https://yongweiwu.wordpress.com/2014/12/07/study-notes-functional-programming-with-cplusplus/comment-page-1/#comment-14257
*/
namespace vol::utility {
   
  auto compose ()
  {
      return [](auto&& x) -> decltype(auto)
      {
          return std::forward<decltype(x)>(x);
      };
  }
   
  template <typename Fn>
  auto compose (Fn fn)
  {
      return [=](auto&&... x) -> decltype(auto)
      {
          return fn(std::forward<decltype(x)>(x)...);
      };
  }
   
  template <typename Fn, typename... Fargs>
  auto compose (Fn fn, Fargs... args)
  {
      return [=](auto&&... x) -> decltype(auto)
      {
          return fn(
              compose(args...)(std::forward<decltype(x)>(x)...));
      };
  }
}

