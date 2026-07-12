#ifndef QUANTITIES_DETAILS_H_INCLUDED
#define QUANTITIES_DETAILS_H_INCLUDED

/*!
  \file Details.h
  \author gbug
  \brief A bunch of helpers to implement Quantities::Traits and Quantities::State.
*/

#include "common/IOMode.h"

#include <tuple>
#include <string>
#include <ostream>
#include <istream>
#include <algorithm>

namespace Quantities::details
{
  template<class T> concept has_ncomps = requires { T::ncomps; };
  template<class T> constexpr int ncomps_of() noexcept
  {
    if constexpr (has_ncomps<T>)
      return T::ncomps;
    else
      return 1;
  }
  template<int N> struct Name
  {
    char data[N] = {};
    consteval operator const char*() const { return data; }
    consteval Name(const char (&name)[N]) { std::copy_n(name, N, data); }
  };

  template<size_t N, class Q, class... Qs>
    struct index_of_impl
    {
      static constexpr size_t value = N;
    };

  template<size_t I, class Q, class Qi, class... Qs>
    struct index_of_impl<I, Q, Qi, Qs...>
    {
      static constexpr size_t value =
        std::is_same_v<Q, Qi>? I : index_of_impl<I + 1, Q, Qs...>::value;
    };

  template<class Q, class... Qs>
    constexpr size_t index_of = index_of_impl<0, Q, Qs...>::value;

  template<size_t I, class... Qs>
    using type_of = std::tuple_element_t<I, std::tuple<Qs...>>;

  template<class... Qs>
    constexpr std::initializer_list<const char *> qnames = {Qs::id...};

  template<class... Qs>
    constexpr bool are_unique = true;

  template<class Q, class... Qs>
    constexpr bool are_unique<Q, Qs...> = ((!std::is_same_v<Q, Qs>) && ...) && are_unique<Qs...>;

/*---------------------------------------------------------------------------------------*/

  template<size_t I = 0, class S>
    void write_state_in_brackets(std::ostream &out, const S &state)
  {
    if constexpr (I != S::ncomps)
    {
      using Q = S::template type_of<I>;
      out << (I? ", " : "{") << Q::id << " " << state.template get<I>();
      write_state_in_brackets<I + 1>(out, state);
    }
  }

  template<size_t I = 0, class S>
    void write_state_bare_comps(std::ostream &out, const S &state)
  {
    if constexpr (I != S::ncomps)
    {
      out << (I? " " : "") << state.template get<I>();
      write_state_bare_comps<I + 1>(out, state);
    }
  }

  template<size_t I = 0, class S>
    void write_state(std::ostream &out, const S &state)
  {
    if (IO::use_brackets(out))
    {
      write_state_in_brackets(out, state);
      out << '}';
    }
    else
      write_state_bare_comps(out, state);
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t I = 0, class S>
    void read_state_in_brackets(std::istream &in, S &state)
  {
    if constexpr (I != S::ncomps)
    {
      using Q = typename S::template type_of<I>;
      if (std::string name; !(in >> name) || name != std::string(Q::id))
      {
        in.setstate(std::ios::failbit);
        return;
      }
      if (!(in >> state.template get<I>()))
        return;
      if (int ch = in.get(); (ch != ',' && I != S::ncomps - 1) || (ch != '}' && I == S::ncomps - 1))
      {
        in.setstate(std::ios::failbit);
        return;
      }
      read_state_in_brackets<I + 1>(in, state);
    }
  }

  template<size_t I = 0, class S>
    void read_state_bare_comps(std::istream &in, S &state)
  {
    if constexpr (I != S::ncomps)
    {
      in >> state.template get<I>();
      read_state_bare_comps<I + 1>(in, state);
    }
  }

  template<size_t I = 0, class S>
    void read_state(std::istream &in, S &state)
  {
    in >> std::ws;
    if (in.peek() == '{')
    {
      in.get();
      read_state_in_brackets(in, state);
    }
    else
      read_state_bare_comps(in, state);
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t I = 0, class L, class R>
    constexpr void add_to(L &l, const R &r) noexcept
  {
    if constexpr (I != L::ncomps)
    {
      using T = L::template type_of<I>;
      static_assert(R::template has<T>, "right-hand state doesn't have a quantity");
      l.template get<T>() += r.template get<T>();
      add_to<I + 1>(l, r);
    }
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t I = 0, class L, class R>
    constexpr void sub_from(L &l, const R &r) noexcept
  {
    if constexpr (I != L::ncomps)
    {
      using T = L::template type_of<I>;
      static_assert(R::template has<T>, "right-hand state doesn't have a quantity");
      l.template get<T>() -= r.template get<T>();
      sub_from<I + 1>(l, r);
    }
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t I = 0, class S, class T>
    constexpr void mult_by(S &state, T value) noexcept
  {
    if constexpr (I != S::ncomps)
    {
      state.template get<I>() *= value;
      mult_by<I + 1>(state, value);
    }
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t I = 0, class S, class T>
    constexpr void div_by(S &state, T value) noexcept
  {
    if constexpr (I != S::ncomps)
    {
      state.template get<I>() /= value;
      div_by<I + 1>(state, value);
    }
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t I = 0, class S, class T>
    constexpr void set_to_value(S &state, T value) noexcept
  {
    if constexpr (I != S::ncomps)
    {
      using Q_t = S::template type_of<I>::type;
      state.template get<I>() = static_cast<Q_t>(value);
      set_to_value<I + 1>(state, value);
    }
  }

  template<size_t I = 0, class L, class R>
    constexpr void set_to_state(L &l, const R &r) noexcept
  {
    if constexpr (I != L::ncomps)
    {
      using T = L::template type_of<I>;
      static_assert(R::template has<T>, "right-hand state doesn't have a quantity");
      l.template get<T>() = r.template get<T>();
      set_to_state<I + 1>(l, r);
    }
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t I = 0, class L, class R>
   constexpr bool equal(const L &l, const R &r) noexcept
  {
    if constexpr (I != L::ncomps)
    {
      using T = L::template type_of<I>;
      static_assert(R::template has<T>, "right-hand state doesn't have a quantity");
      return (l.template get<T>() == r.template get<T>()) && equal<I + 1>(l, r);
    }
    else
      return true;
  }

} // namespace Quantities::details

#endif // QUANTITIES_DETAILS_H_INCLUDED
