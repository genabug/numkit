#ifndef QUANTITIES_STATE_H_INCLUDED
#define QUANTITIES_STATE_H_INCLUDED

/*!
  \file State.h
  \author gbug
  \brief Vector of quantities with arbitrary types, definition, documentation and basic tests.
*/

#include "Traits.h"

namespace Quantities
{
  template<IsTraits... Qs> class State;

  namespace details
  {
    template<class> struct is_state : std::false_type {};
    template<class... Qs> struct is_state<State<Qs...>> : std::true_type {};
  }

  template<class S> concept IsState = details::is_state<S>::value;

  template<IsTraits... Qs> class State
  {
    static_assert(sizeof...(Qs) > 0, "state with zero traits is not allowed"); // why?
    static_assert(details::are_unique<Qs...>, "traits must be unique in a state");
    std::tuple<typename Qs::type...> data = {};

  public:
    // traits
    static constexpr int ncomps = sizeof...(Qs);

    // helpers
    template<size_t I> using type_of = details::type_of<I, Qs...>;
    template<IsTraits Q> static constexpr auto index_of = details::index_of<Q, Qs...>;
    template<IsTraits Q> static constexpr bool has = index_of<Q> < ncomps;

    // ctors
    constexpr State() noexcept = default;
    constexpr State(IsState auto &&s) noexcept : data(std::forward(s).template get<Qs>()...) {}
    constexpr State(const IsState auto &s) noexcept : data(s.template get<Qs>()...) {}

    template<class... Args> requires(sizeof...(Args) == sizeof...(Qs))
      constexpr explicit State(Args... args) noexcept : data(args...) {}

    // ops
    constexpr State& operator=(auto) noexcept;
    constexpr State& operator*=(auto) noexcept;
    constexpr State& operator/=(auto) noexcept;
    constexpr State& operator+=(const IsState auto &) noexcept;
    constexpr State& operator-=(const IsState auto &) noexcept;

    constexpr State operator-() const noexcept;
    constexpr State operator+() const noexcept { return *this; }

    // access by index (mainly to implement basic ops, see details)
    template<size_t I> requires(I < ncomps) constexpr auto& get() & noexcept { return std::get<I>(data); }
    template<size_t I> requires(I < ncomps) constexpr auto& get() const & noexcept { return std::get<I>(data); }
    template<size_t I> requires(I < ncomps) constexpr auto&& get() && noexcept { return std::get<I>(std::move(data)); }
    template<size_t I> requires(I < ncomps) constexpr auto&& get() const && noexcept { return std::get<I>(std::move(data)); }

    // access by type-name (for generic code)
    template<IsTraits Q> requires(has<Q>) constexpr auto& get() & noexcept { return get<index_of<Q>>(); };
    template<IsTraits Q> requires(has<Q>) constexpr auto& get() const & noexcept { return get<index_of<Q>>(); };
    template<IsTraits Q> requires(has<Q>) constexpr auto&& get() && noexcept { return std::move(*this).template get<index_of<Q>>(); };
    template<IsTraits Q> requires(has<Q>) constexpr auto&& get() const && noexcept { return std::move(*this).template get<index_of<Q>>(); };

    // index access by variable (for end-user code)
    template<IsTraits Q> constexpr auto& operator[](Q) & noexcept { return get<Q>(); }
    template<IsTraits Q> constexpr auto& operator[](Q) const & noexcept { return get<Q>(); }
    template<IsTraits Q> constexpr auto&& operator[](Q) && noexcept { return std::move(*this).template get<Q>(); }
    template<IsTraits Q> constexpr auto&& operator[](Q) const && noexcept { return std::move(*this).template get<Q>(); }

    // TODO: member access, see https://stackoverflow.com/q/54617101/8802124
    // Not possible in standard C++ today, needs static reflection.
  }; // class State<Qs...>

  // IO operations
  std::istream& operator>>(std::istream &, IsState auto &);
  std::ostream& operator<<(std::ostream &, const IsState auto &);

  // arithmetic operations
  // NB! operations are not symmetric, right-hand operand must has all the components
  // of the left-hand operand, otherwise an operation is not compilable.
  constexpr auto operator+(const IsState auto &, const IsState auto &) noexcept;
  constexpr auto operator-(const IsState auto &, const IsState auto &) noexcept;

  constexpr auto operator*(const IsState auto &, auto) noexcept;
  constexpr auto operator*(auto, const IsState auto &) noexcept;
  constexpr auto operator/(const IsState auto &, auto) noexcept;

  // boolean operations
  // NB! operations are asymmetric.
  constexpr bool operator==(const IsState auto &, const IsState auto &) noexcept;
  constexpr bool operator!=(const IsState auto &, const IsState auto &) noexcept;
} // namespace Quantities

/*---------------------------------------------------------------------------------------*/
/*------------------------------------ definitions --------------------------------------*/
/*---------------------------------------------------------------------------------------*/

namespace Quantities
{
  template<IsTraits... Qs>
    constexpr State<Qs...>& State<Qs...>::operator=(auto v) noexcept
  {
    if constexpr (IsState<decltype(v)>)
      details::set_to_state(*this, v);
    else
      details::set_to_value(*this, v);
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<IsTraits... Qs>
    constexpr State<Qs...>& State<Qs...>::operator+=(const IsState auto &s) noexcept
  {
    details::add_to(*this, s);
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<IsTraits... Qs>
    constexpr State<Qs...>& State<Qs...>::operator-=(const IsState auto &s) noexcept
  {
    details::sub_from(*this, s);
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<IsTraits... Qs> template<class T>
    constexpr State<Qs...>& State<Qs...>::operator*=(T value) noexcept
  {
    details::mult_by(*this, value);
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<IsTraits... Qs> template<class T>
    constexpr State<Qs...>& State<Qs...>::operator/=(T value) noexcept
  {
    details::div_by(*this, value);
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<IsTraits... Qs>
    constexpr State<Qs...> State<Qs...>::operator-() const noexcept
  {
    auto s = *this;
    details::mult_by(s, -1);
    return s;
  }

/*---------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------*/

  std::ostream& operator<<(std::ostream &ostr, const IsState auto &s)
  {
    details::write_state(ostr, s);
    return ostr;
  }

/*---------------------------------------------------------------------------------------*/

  std::istream& operator>>(std::istream &istr, IsState auto &s)
  {
    details::read_state(istr, s);
    return istr;
  }

/*---------------------------------------------------------------------------------------*/

  constexpr auto operator+(const IsState auto &l, const IsState auto &r) noexcept
  {
    auto s = l;
    details::add_to(s, r);
    return s;
  }

/*---------------------------------------------------------------------------------------*/

  constexpr auto operator-(const IsState auto &l, const IsState auto &r) noexcept
  {
    auto s = l;
    details::sub_from(s, r);
    return s;
  }

/*---------------------------------------------------------------------------------------*/

  constexpr auto operator*(const IsState auto &s, auto v) noexcept
  {
    auto r = s;
    details::mult_by(r, v);
    return r;
  }

  constexpr auto operator*(auto v, const IsState auto &s) noexcept
  {
    return s * v;
  }

/*---------------------------------------------------------------------------------------*/

  constexpr auto operator/(const IsState auto &s, auto v) noexcept
  {
    auto r = s;
    details::div_by(r, v);
    return r;
  }

/*---------------------------------------------------------------------------------------*/

  constexpr bool operator==(const IsState auto &l, const IsState auto &r) noexcept
  {
    return details::equal(l, r);
  }

  constexpr bool operator!=(const IsState auto &l, const IsState auto &r) noexcept
  {
    return !(l == r);
  }
} // namespace Quantities

/*---------------------------------------------------------------------------------------*/
/*--------------------------------------- tests -----------------------------------------*/
/*---------------------------------------------------------------------------------------*/

namespace Quantities::tests
{
  using t1 = Traits<int, 3, "ti">;
  using t2 = Traits<double, 3, "td">;

  constexpr t1 ti;
  constexpr t2 td;

  using S = State<t1, t2>;
  using t3 = Traits<float, 3, "tf">;
  static_assert(S::has<t1>);
  static_assert(S::has<t2>);
  static_assert(!S::has<t3>);
  static_assert(S::index_of<t1> == 0);
  static_assert(S::index_of<t2> == 1);
  static_assert(S::index_of<t3> == S::ncomps);
  static_assert(std::is_same_v<t1, S::type_of<0>>);
  static_assert(std::is_same_v<t2, S::type_of<1>>);

  constexpr State<t1, t2> s(1, 2);
  static_assert(s.get<0>() == 1 && s.get<1>() == 2);
  static_assert(s.get<t1>() == 1 && s.get<t2>() == 2);
  static_assert(s[ti] == 1 && s[td] == 2);

  static_assert(+State<t1, t2>(1, 2) == State<t1, t2>(+1, +2));
  static_assert(-State<t1, t2>(1, 2) == State<t1, t2>(-1, -2));

  static_assert(State<t1, t2>(1, 2) * 2 == State<t1, t2>(2, 4));
  static_assert(2 * State<t1, t2>(3, 4) == State<t1, t2>(6, 8));
  static_assert(State<t1, t2>(2, 4) / 2 == State<t1, t2>(1, 2));

  static_assert(State<t1, t2>(1, 2) == State<t1, t2>(1, 2));
  static_assert(State<t1, t2>(1, 2) == State<t2, t1>(2, 1));
  static_assert(State<t1>(1) == State<t2, t1>(2, 1));
  static_assert(State<t1>(2) != State<t2, t1>(2, 1));

  static_assert(State<t1, t2>(1, 2) + State<t1, t2>(3, 4) == State<t1, t2>(4, 6));
  static_assert(State<t1, t2>(1, 2) + State<t2, t1>(3, 4) == State<t1, t2>(5, 5));
  static_assert(State<t1>(1) + State<t1, t2>(3, 4) == State<t1>(4));

  static_assert(State<t1, t2>(3, 4) - State<t1, t2>(2, 1) == State<t1, t2>(1, 3));
  static_assert(State<t1, t2>(3, 4) - State<t2, t1>(2, 1) == State<t1, t2>(2, 2));
  static_assert(State<t1>(2) - State<t1, t2>(1, 3) == State<t1>(1));
} // namespace Quantities::tests

/*---------------------------------------------------------------------------------------*/
/*----------------------------------- documentation -------------------------------------*/
/*---------------------------------------------------------------------------------------*/

/*!
  \class Quantities::State
  \tparam Qs Type-names (traits) of quantities.
  \brief Vector of quantities with arbitrary types.

  Class is aimed for aggregation and processing an arbitrary set of heterogeneous
  values as a single structure with a set of basic operations (arithmetic, logical, IO)
  on objects of such classes. Set of values of physical nature can be considered
  as a state of matter at some spatial point thus the class is called State.
  Basic arithmetic operations with such states allows us to consider them as vectors
  e.g. for flux computations.

  Quantity's traits (\see Quantities::Traits) must be defined in order to build a state.
  \code
  using w_t = Traits<Vector3D, 2, "w">; // ncomps == 3
  using rho_t = Traits<double, 3, "rho">; // ncomps == 1

  constexpr w_t w;
  constexpr rho_t rho;
  \endcode
  In the example above vector of velocity ("w") vector is defined on faces (Dim=2)
  and scalar density ("rho") is defined in cells (Dim=3).
  \c w_t and \c rho_t are now so-called type-names which are used to build the state.
  They also used for access the corresponding values in a state
  though it's recommended to define variable-names \c w and \c rho for access.
  Variable-names are supposed to be used in end-user code like solvers,
  while type-names are intended to be used in generic code like approximations.

  When traits are defined, a state can be made:
  \code
  using HD2T_s = State<rho_t, Te_t, Ti_t, w_t>;
  HD2T_s hd1(1e-6, 1e-3, 1e-3, V3d(0));
  auto hd2 = 2*hd1;
  hd1[rho] = 2e-6;
  auto hd3 = (hd1 + hd2) / 2;
  std::cerr << hd3 << '\n';
  \endcode

  There are three ways to get the components of a state: by index, by type-name
  and a variable of the corresponding type-name. The last one is shorter
  and uses less parentheses but requires the global variables.
  \code
  s1[rho] = 4;
  //s1.rho = 4; // this would be perfect (with reflection from c++26?)
  \endcode
  It's possible to get subset of components at once (slice)
  by simply assigning a state to another state with required quantities:
  \code
  State<rho_t, Ti_t, w_t> hd4 = hd1;
  \endcode

  Class is accompanying with a set of arithmetic, logical and IO operations.
  Note that both arithmetic and logical operations are not symmetric!
  This is done in order to allow these operations on states with different sets of components,
  when one set is a subset of the other, or when the order of components is differ.
  \code
  hd3 = (hd1 + hd2) / 2; // hd{1,2,3} are the same type, HD2T_s
  hd4 = (hd1 + hd2) / 2; // it's also OK, both hd{1,2} have all needed components
  //hd1 = hd4; // COMPILE ERROR: Te_t is not presented in the hd4 state
  \endcode

  All operations with states are constexpr and can be done in compile-time,
  thus most of the time misusage of working with states leads to a compilation error.
  For example, access to a component which is not presented in a state,
  mixing states with unmatched list of quantities, etc...

  \see details.h for implementation details.
  \see Traits.h for quantities traits implementation.
*/

#endif // QUANTITIES_STATE_H_INCLUDED
