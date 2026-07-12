#ifndef MATH_TYPE_H_INCLUDED
#define MATH_TYPE_H_INCLUDED

/*!
  \file Type.h
  \author gbug
  \brief Concept for types eligible in numerical computations.
*/

#include <concepts>
#include <type_traits>

namespace Math
{
  template<class T>
  concept Type = std::is_default_constructible_v<T>
    && std::is_copy_constructible_v<T>
    && requires(T a, T b) {
      { a += b } -> std::convertible_to<T&>;
      { a -= b } -> std::convertible_to<T&>;
      { a *= b } -> std::convertible_to<T&>;
      { a /= b } -> std::convertible_to<T&>;
      { a == b } -> std::convertible_to<bool>;
    };
} // namespace Math

/*---------------------------------------------------------------------------------------*/
/*------------------------------------ documentation ------------------------------------*/
/*---------------------------------------------------------------------------------------*/

/*!
  \interface Math::Type
  \brief Constrains types eligible for use in numerical computations.

  A type satisfies \c Type if it is default-constructible, copy-constructible,
  and supports compound arithmetic assignment operators (\c +=, \c -=, \c *=, \c /=)
  as well as equality comparison (\c ==).
  This is intentionally broader than \c std::is_arithmetic: user-defined types
  such as vectors, tensors, or any other numeric-like type that implements
  these operators will satisfy the concept automatically.

  \see Quantities::Traits
  \see Quantities::State
*/

#endif // MATH_TYPE_H_INCLUDED

