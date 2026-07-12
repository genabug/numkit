#ifndef MATH_DETAILS_H_INCLUDED
#define MATH_DETAILS_H_INCLUDED

/*!
  \file details.h
  \author gbug
  \brief Various helpers
*/

#include <limits>
#include <cstddef>
#include <concepts>
#include <type_traits>

namespace Math::details
{
  // constexpr version of std::abs
  // TODO: remove after switching to c++23
  constexpr auto abs(auto x) noexcept { return (x < 0)? -x : x; }

  // constexpr version of std::sqrt for both integral and floating point types
  // TODO: remove after switching to c++26
  constexpr auto sqrt(std::integral auto x) noexcept;
  constexpr auto sqrt(std::floating_point auto x) noexcept;

  // floating-point comparison with specific epsilon
  template<std::floating_point T>
    constexpr bool fp_equal(T x, T y, size_t ulp = 1) noexcept;
} // namespace Math::details

/*---------------------------------------------------------------------------------------*/
/*------------------------------------ definitions --------------------------------------*/
/*---------------------------------------------------------------------------------------*/

namespace Math::details::impl
{
  template<std::integral T> constexpr T sqrt_int(T x, T lo, T hi) noexcept
  {
    auto mid = (lo + hi + 1) / 2;
    if (lo == hi)
      return lo;
    else
      return (x / mid < mid)? sqrt_int(x, lo, mid - 1) : sqrt_int(x, mid, hi);
  }
  template<std::floating_point T> constexpr T sqrt_real(T x, T lo, T hi) noexcept
  {
    return (lo == hi) ? lo : sqrt_real(x, T{0.5} * (lo + x / lo), lo);
  }
}

constexpr auto Math::details::sqrt(std::floating_point auto x) noexcept
{
  using type = std::decay_t<decltype(x)>;
  return (x < type{0})? type{-1} : impl::sqrt_real(x, x, type{0});
}

constexpr auto Math::details::sqrt(std::integral auto x) noexcept
{
  using type = std::decay_t<decltype(x)>;
  return (x < type{0})? type{-1} : impl::sqrt_int(x, type{0}, x / 2 + type{1});
}

/*---------------------------------------------------------------------------------------*/

template<std::floating_point T>
  constexpr bool Math::details::fp_equal(T x, T y, size_t ulp) noexcept
{
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units of least precision)
  return abs(x - y) < std::numeric_limits<T>::epsilon() * abs(x + y) * ulp
         // unless the result is subnormal
         || abs(x - y) < std::numeric_limits<T>::min();
}

/*---------------------------------------------------------------------------------------*/
/*--------------------------------------- tests -----------------------------------------*/
/*---------------------------------------------------------------------------------------*/

namespace Math::details::tests
{
  static_assert(abs(-1) == 1, "abs failed");
  static_assert(abs(1.) == 1., "abs failed");

  static_assert(sqrt(-1) == -1, "sqrt failed");
  static_assert(sqrt(-2.) == -1., "sqrt failed");
  static_assert(sqrt(-3.f) == -1.f, "sqrt failed");
  static_assert(sqrt(0) == 0, "sqrt failed");
  static_assert(sqrt(0.) == 0., "sqrt failed");
  static_assert(sqrt(0.f) == 0.f, "sqrt failed");
  static_assert(sqrt(1) == 1, "sqrt failed");
  static_assert(sqrt(1.) == 1., "sqrt failed");
  static_assert(sqrt(1.f) == 1.f, "sqrt failed");
  static_assert(sqrt(4) == 2, "sqrt failed");
  static_assert(sqrt(4.) == 2., "sqrt failed");
  static_assert(sqrt(4.f) == 2.f, "sqrt failed");

  static_assert(fp_equal(6.022140857e+23, 6.022140857e+23 + 2e8), "fp equal failed");
  static_assert(!fp_equal(6.022140857e+23, 6.022140857e+23 + 3e8), "fp equal failed");
}

#endif // MATH_DETAILS_H_INCLUDED
