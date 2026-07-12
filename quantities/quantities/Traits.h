#ifndef QUANTITIES_TRAITS_H_INCLUDED
#define QUANTITIES_TRAITS_H_INCLUDED

/*!
  \file Traits.h
  \author gbug
  \brief Quantity's traits, definition, documentation and tests.
*/

#include "details.h"
#include "math/Type.h"

namespace Quantities
{
  template<Math::Type T, int Dim, details::Name name> struct Traits
  {
    using type = T;
    static constexpr int dim = Dim;
    static constexpr auto id = name;
    static constexpr int ncomps = details::ncomps_of<T>();
  };

  namespace details
  {
    template<class> struct is_traits : std::false_type {};
    template<Math::Type T, int Dim, Name name> struct is_traits<Traits<T, Dim, name>> : std::true_type {};
  }

  template<class T> concept IsTraits = details::is_traits<T>::value;
} // namespace Quantities

/*---------------------------------------------------------------------------------------*/
/*--------------------------------------- tests -----------------------------------------*/
/*---------------------------------------------------------------------------------------*/

namespace Quantities::tests
{
  using rho_t = Traits<int, 11, "rho">;
  static_assert(std::is_same_v<rho_t::type, int>);
  static_assert(rho_t::dim == 11);
  static_assert(rho_t::ncomps == 1);
  static_assert(std::string_view(rho_t::id) == "rho");

  struct HD
  {
    double rho, P, w;
    static constexpr int ncomps = 3;
    constexpr bool operator==(const HD &) const = default;
    constexpr HD& operator+=(const HD &o) { rho += o.rho; P += o.P; w += o.w; return *this; }
    constexpr HD& operator-=(const HD &o) { rho -= o.rho; P -= o.P; w -= o.w; return *this; }
    constexpr HD& operator*=(const HD &o) { rho *= o.rho; P *= o.P; w *= o.w; return *this; }
    constexpr HD& operator/=(const HD &o) { rho /= o.rho; P /= o.P; w /= o.w; return *this; }
  };
  static_assert(Math::Type<HD>);

  using HD_t = Traits<HD, 22, "HD">;
  static_assert(std::is_same_v<HD_t::type, HD>);
  static_assert(HD_t::dim == 22);
  static_assert(HD_t::ncomps == HD::ncomps);
  static_assert(std::string_view(HD_t::id) == "HD");

} // namespace Quantities::tests

/*---------------------------------------------------------------------------------------*/
/*------------------------------------ documentation ------------------------------------*/
/*---------------------------------------------------------------------------------------*/

/*!
  \interface Quantities::Type
  \brief Alias for Math::Type, constrains types eligible for use in Quantities::Traits.
  \see Math::Type
*/

/*!
  \class Quantities::Traits
  \tparam T Type of the quantity's data, see Math::Type.
  \tparam Dim Dimension of the mesh elements where its data are defined.
  \tparam Name String ID of the quantity.
  \brief Type traits of a quantity.

  Quantity's traits is the main identifier of a quantity within the system.
  They are used to create and access the values of a state, see \Quantities::State.
  They are also used to implement all kinds of additional utilities
  like (de)serialization, arithmetic and boolean operations.

  Apart from user-defined traits the class contains an additional one, four in all:
  * type (user-defined): type of the quantitiy, usually double, Vector3D or even State
  * dim (user-defined): dimension of mesh elements where data are defined
  * id (user-defined): string ID of the quantity
  * ncomps (auto): number of quantity's components

  Traits::ncomps is a number of components of the quantity. For a simple quantity it's 1,
  while for a compound one it's greater than 1. It's defined based on whether the field
  of the same name is presented in Type (then it equals to it) or not (and then it's 1).
*/

#endif // QUANTITIES_TRAITS_H_INCLUDED
