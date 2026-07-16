#ifndef MATH_TENSOR_H_INCLUDED
#define MATH_TENSOR_H_INCLUDED

/*!
  \file Tensor.h
  \author gbug
  \brief Tensor of rank 2 with arbitrary type, definition, documentation and tests.
*/

#include "Vector.h"

namespace Math
{
  template<size_t N, Type T = double> class Tensor
  {
    std::array<T, N*N> data = {};
    static_assert(N != 0, "Tensor of zero size is meaningless.");

  public:
    // traits
    static constexpr int ncomps = N*N;

    constexpr auto begin() noexcept { return data.begin(); }
    constexpr auto end() noexcept { return data.end(); }
    constexpr auto begin() const noexcept { return data.begin(); }
    constexpr auto end() const noexcept { return data.end(); }

    // ctors
    constexpr explicit Tensor() noexcept = default;
    template<class U> requires(std::is_convertible_v<U, T>)
      constexpr explicit Tensor(const U &a) noexcept;
    template<class... Ts> requires(sizeof...(Ts) == N)
      constexpr explicit Tensor(const Ts&... as) noexcept;
    template<class... Ts> requires(sizeof...(Ts) == N*N)
      constexpr explicit Tensor(const Ts&... as) noexcept;

    // converters
    template<Type U> constexpr explicit Tensor(const Tensor<N, U> &t) noexcept;
    template<Type U> constexpr Tensor& operator=(const Tensor<N, U> &t) noexcept;

    // access
    constexpr T operator[](size_t i, size_t j) && noexcept { assert(i < N && j < N); return data[i*N + j]; }
    constexpr T& operator[](size_t i, size_t j) & noexcept { assert(i < N && j < N); return data[i*N + j]; }
    constexpr const T& operator[](size_t i, size_t j) const & noexcept { assert(i < N && j < N); return data[i*N + j]; }

    // unary ops (NB! returns a copy)
    constexpr Tensor operator-() const noexcept;
    constexpr Tensor operator+() const noexcept { return *this; }
    constexpr Tensor operator~() const noexcept { return transpose(); }

    // assign with op
    constexpr Tensor& operator*=(const T &a) noexcept;
    constexpr Tensor& operator/=(const T &a) noexcept;
    constexpr Tensor& operator+=(const Tensor &A) noexcept;
    constexpr Tensor& operator-=(const Tensor &A) noexcept;
    constexpr Tensor& operator*=(const Tensor &A) noexcept;
    constexpr Tensor& operator/=(const Tensor &A) noexcept { return *this *= A.invert(); }

    // comparison ops
    constexpr bool operator==(const Tensor &) const noexcept = default;

    // other useful ops
    constexpr T det() const noexcept;
    constexpr T trace() const noexcept;
    constexpr Tensor invert() const noexcept;
    constexpr Tensor transpose() const noexcept;

  private:
    constexpr Tensor<N-1, T> M(size_t I, size_t J) const noexcept;
  }; // class Tensor<N, T>

  using Tensor2D = Tensor<2>;
  using Tensor3D = Tensor<3>;

/*---------------------------------------------------------------------------------------*/

  // arithmetic ops
  template<size_t N, Type T>
    constexpr auto operator+(Tensor<N, T> A, const Tensor<N, T> &B) noexcept { A += B; return A; }

  template<size_t N, Type T>
    constexpr auto operator-(Tensor<N, T> A, const Tensor<N, T> &B) noexcept { A -= B; return A; }

  template<size_t N, Type T>
    constexpr auto operator*(Tensor<N, T> A, const Tensor<N, T> &B) noexcept { A *= B; return A; }

  template<size_t N, Type T>
    constexpr auto operator*(Tensor<N, T> A, const T &a) noexcept { A *= a; return A; }

  template<size_t N, Type T>
    constexpr auto operator*(const T &a, Tensor<N, T> A) noexcept { A *= a; return A; }

  template<size_t N, Type T>
    constexpr auto operator/(Tensor<N, T> A, const T &a) noexcept { A /= a; return A; }

  template<size_t N, Type T>
    constexpr auto operator/(Tensor<N, T> A, const Tensor<N, T> &B) noexcept { A /= B; return A; }

  // io ops
  template<size_t N, Type T>
    std::istream& operator>>(std::istream &in, Tensor<N, T> &A);

  template<size_t N, Type T>
    std::ostream& operator<<(std::ostream &out, const Tensor<N, T> &A);

  // ops with vectors
  template<size_t N, Type T>
    constexpr auto& operator*=(Vector<N, T> &a, const Tensor<N, T> &A) noexcept;

  template<size_t N, Type T>
    constexpr auto operator*(Vector<N, T> a, const Tensor<N, T> &A) noexcept { a *= A; return a; }

  template<size_t N, Type T>
    constexpr auto operator*(const Tensor<N, T> &A, Vector<N, T> a) noexcept { a *= ~A; return a; }

  template<size_t N, Type T>
    constexpr auto& operator/=(Vector<N, T> &a, const Tensor<N, T> &A) noexcept { return a *= A.invert(); }

  template<size_t N, Type T>
    constexpr auto operator/(Vector<N, T> a, const Tensor<N, T> &A) noexcept { a /= A; return a; }

  template<size_t N, Type T>
    constexpr auto operator^(const Vector<N, T> &a, const Vector<N, T> &b) noexcept;

  // ops with 2D vectors
  template<Type T>
    constexpr auto operator%(const Tensor<2, T> &A, const Vector<2, T> &a) noexcept;

  template<Type T>
    constexpr auto operator%(const Vector<2, T> &a, const Tensor<2, T> &A) noexcept;

  template<Type T>
    constexpr auto operator%(const Tensor<2, T> &A, const Tensor<2, T> &B) noexcept;

  // ops with 3D vectors
  template<Type T> constexpr Tensor<3, T> operator~(const Vector<3, T> &a) noexcept;

  template<Type T>
    constexpr auto operator%(const Tensor<3, T> &A, const Vector<3, T> &a) noexcept;

  template<Type T>
    constexpr auto operator%(const Vector<3, T> &a, const Tensor<3, T> &A) noexcept;

/*---------------------------------------------------------------------------------------*/
/*------------------------------------ definition ---------------------------------------*/
/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T> template<class U> requires(std::is_convertible_v<U, T>)
    constexpr Tensor<N, T>::Tensor(const U &a) noexcept
  {
    for (size_t i = 0; i < N; ++i)
      data[i*N + i] = static_cast<T>(a);
  }

  template<size_t N, Type T> template<class... Ts> requires(sizeof...(Ts) == N)
    constexpr Tensor<N, T>::Tensor(const Ts&... as) noexcept
  {
    const std::array<T, N> arr{as...};
    for (size_t i = 0; i < N; ++i)
      data[i*N + i] = static_cast<T>(arr[i]);
  }

  template<size_t N, Type T> template<class... Ts> requires(sizeof...(Ts) == N*N)
    constexpr Tensor<N, T>::Tensor(const Ts&... as) noexcept : data{as...} {}

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    template<Type U> constexpr Tensor<N, T>::Tensor(const Tensor<N, U> &t) noexcept
  {
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        data[i*N + j] = static_cast<T>(t[i, j]);
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    template<Type U> constexpr Tensor<N, T>& Tensor<N, T>::operator=(const Tensor<N, U> &t) noexcept
  {
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        data[i*N + j] = static_cast<T>(t[i, j]);
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr Tensor<N, T> Tensor<N, T>::operator-() const noexcept
  {
    Tensor<N, T> A;
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        A[i, j] = -data[i*N + j];
    return A;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr Tensor<N, T>& Tensor<N, T>::operator*=(const T &a) noexcept
  {
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        data[i*N + j] *= a;
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr Tensor<N, T>& Tensor<N, T>::operator/=(const T &a) noexcept
  {
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        data[i*N + j] /= a;
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr Tensor<N, T>& Tensor<N, T>::operator+=(const Tensor<N, T> &A) noexcept
  {
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        data[i*N + j] += A[i, j];
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr Tensor<N, T>& Tensor<N, T>::operator-=(const Tensor<N, T> &A) noexcept
  {
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        data[i*N + j] -= A[i, j];
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr Tensor<N, T>& Tensor<N, T>::operator*=(const Tensor<N, T> &A) noexcept
  {
    auto B = *this;
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
      {
        data[i*N + j] = 0;
        for (size_t k = 0; k < N; ++k)
          data[i*N + j] += B[i, k]*A[k, j];
      }
    return *this;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T> constexpr T Tensor<N, T>::trace() const noexcept
  {
    T tr = 0;
    for (size_t i = 0; i < N; ++i)
      tr += data[i*N + i];
    return tr;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T> constexpr Tensor<N, T> Tensor<N, T>::invert() const noexcept
  {
    // TODO: possible issues:
    // 1. exact equality check (d == 0) is unreliable for floating-point types;
    //    consider using a tolerance-based comparison or std::abs(d) < epsilon
    // 2. silently returns zero tensor on singular matrix, which may cause subtle bugs;
    //    consider throwing an exception or returning std::optional<Tensor>
    T d = det();
    if (d == static_cast<T>(0))
      return Tensor<N, T>(0); // inverse matrix doesn't exist, return 0

    Tensor<N, T> A;
    T c = static_cast<T>(1) / d;
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        A[j, i] = ( ((i + j) & 1)? -c : c ) * M(i, j).det();

    return A;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T> constexpr Tensor<N, T> Tensor<N, T>::transpose() const noexcept
  {
    Tensor<N, T> A;
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        A[i, j] = data[j*N + i];
    return A;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr Tensor<N-1, T> Tensor<N, T>::M(size_t I, size_t J) const noexcept
  {
    Tensor<N-1, T> A;
    for (size_t i = 0; i < N-1; ++i)
      for (size_t j = 0; j < N-1; ++j)
      {
        size_t im = (i < I)? i : i+1;
        size_t jm = (j < J)? j : j+1;
        A[i, j] = data[im*N + jm];
      }
    return A;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr auto& operator*=(Vector<N, T> &a, const Tensor<N, T> &A) noexcept
  {
    auto b = a;
    for (size_t i = 0; i < N; ++i)
    {
      a[i] = 0;
      for (size_t j = 0; j < N; ++j)
        a[i] += b[j] * A[j, i];
    }
    return a;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type U>
    std::istream& operator>>(std::istream &in, Tensor<N, U> &A)
  {
    IO::read_values(in, A, '[', ']');
    return in;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type U>
    std::ostream& operator<<(std::ostream &out, const Tensor<N, U> &A)
  {
    IO::write_values(out, A, '[', ']');
    return out;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr auto operator^(const Vector<N, T> &a, const Vector<N, T> &b) noexcept
  {
    Tensor<N, T> t;
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        t[i, j] = a[i] * b[j];
    return t;
  }

/*---------------------------------------------------------------------------------------*/

  template<Type T>
    constexpr auto operator%(const Tensor<2, T> &A, const Vector<2, T> &a) noexcept
  {
    return Vector<2, T>(A[0, 0] * a[1] - A[0, 1] * a[0],
                        A[1, 0] * a[1] - A[1, 1] * a[0]);
  }

/*---------------------------------------------------------------------------------------*/

  template<Type T>
    constexpr auto operator%(const Vector<2, T> &a, const Tensor<2, T> &A) noexcept
  {
    return Vector<2, T>(a[0] * A[1, 0] - a[1] * A[0, 0],
                        a[0] * A[1, 1] - a[1] * A[0, 1]);
  }

/*---------------------------------------------------------------------------------------*/

  template<Type T>
    constexpr auto operator%(const Tensor<2, T> &A, const Tensor<2, T> &B) noexcept
  {
    Tensor<2, T> R;
    R[0, 0] = A[0, 0] * B[1, 0] - A[0, 1] * B[0, 0];
    R[0, 1] = A[0, 0] * B[1, 1] - A[0, 1] * B[0, 1];
    R[1, 0] = A[1, 0] * B[1, 0] - A[1, 1] * B[0, 0];
    R[1, 1] = A[1, 0] * B[1, 1] - A[1, 1] * B[0, 1];
    return R;
  }

/*---------------------------------------------------------------------------------------*/

  template<Type T> constexpr Tensor<3, T> operator~(const Vector<3, T> &a) noexcept
  {
    return Tensor<3, T>(    0,  -a[2],  a[1],
                         a[2],     0, -a[0],
                        -a[1],  a[0],    0);
  }

/*---------------------------------------------------------------------------------------*/

  template<Type T>
    constexpr auto operator%(const Tensor<3, T> &A, const Vector<3, T> &a) noexcept
  {
    Tensor<3, T> R;
    for (size_t i = 0; i < 3; ++i)
    {
      R[i, 0] = A[i, 1] * a[2] - A[i, 2] * a[1];
      R[i, 1] = A[i, 2] * a[0] - A[i, 0] * a[2];
      R[i, 2] = A[i, 0] * a[1] - A[i, 1] * a[0];
    }
    return R;
  }

/*---------------------------------------------------------------------------------------*/

  template<Type T>
    constexpr auto operator%(const Vector<3, T> &a, const Tensor<3, T> &A) noexcept
  {
    Tensor<3, T> R;
    for (size_t i = 0; i < 3; ++i)
    {
      R[0, i] = a[1] * A[2, i] - a[2] * A[1, i];
      R[1, i] = a[2] * A[0, i] - a[0] * A[2, i];
      R[2, i] = a[0] * A[1, i] - a[1] * A[0, i];
    }
    return R;
  }

/*---------------------------------------------------------------------------------------*/

  template<size_t N, Type T>
    constexpr T Tensor<N, T>::det() const noexcept
  {
    if constexpr (N == 1)
      return data[0];
    else if constexpr (N == 2)
      return data[0]*data[3] - data[1]*data[2];
    else if constexpr (N == 3)
      return data[0] * (data[4]*data[8] - data[5]*data[7]) +
             data[1] * (data[5]*data[6] - data[3]*data[8]) +
             data[2] * (data[3]*data[5] - data[4]*data[6]);
    else // generic case N > 3
    {
      T d = 0;
      for (size_t j = 0; j < N; ++j)
        // coeff: -1 if odd, 1 else,
        // (j & 1) returns 1 if odd, else 0; x << 1 ~ 2*x
        d += (1 - ((j & 1) << 1)) * data[j] * M(0, j).det();
      return d;
    }
  }

} // namespace Math

/*---------------------------------------------------------------------------------------*/
/*--------------------------------------- tests -----------------------------------------*/
/*---------------------------------------------------------------------------------------*/

namespace Math::Tensors::tests
{
  static_assert(std::ranges::range<Tensor3D>, "tensor is not a range");

  using T2d = Tensor<2>;
  using T2i = Tensor<2, int>;
  using T3i = Tensor<3, int>;

  using V2i = Vector<2, int>;
  using V3i = Vector<3, int>;

  // init & access
  constexpr T2i Z(0), E(1), t1(1, 2), t2(3, 4, 5, 6);
  static_assert((Z[0, 0] == 0) && (Z[0, 1] == 0) && (Z[1, 0] == 0) && (Z[1, 1] == 0), "default init failed");
  static_assert((E[0, 0] == 1) && (E[0, 1] == 0) && (E[1, 0] == 0) && (E[1, 1] == 1), "single init failed");
  static_assert((t1[0, 0] == 1) && (t1[0, 1] == 0) && (t1[1, 0] == 0) && (t1[1, 1] == 2), "diagonal init failed");
  static_assert((t2[0, 0] == 3) && (t2[0, 1] == 4) && (t2[1, 0] == 5) && (t2[1, 1] == 6), "full init failed");

  // conversion
  constexpr T2d d(1., 2., 3., 4.);
  constexpr T2i m = T2i(d);
  static_assert((m[0, 0] == 1) && (m[0, 1] == 2) && (m[1, 0] == 3) && (m[1, 1] == 4), "conversion d -> i failed");

  // ops
  static_assert(Z == Z, "t == t failed");
  static_assert(Z != E, "t != t failed");

  static_assert(Z + Z == Z, "Z + Z failed");
  static_assert(Z + t1 == t1, "Z + t failed");
  static_assert(t1 + Z == t1, "t + Z failed");
  static_assert(t1 + t1 == T2i(2, 4), "t + t failed");
  static_assert(t1 + t2 == T2i(4, 4, 5, 8), "t1 + t2 failed");

  static_assert(t2 - t2 == Z, "t - t failed");
  static_assert(t2 - Z == t2, "t - Z failed");
  static_assert(Z - t2 == -t2, "Z - t failed");
  static_assert(t2 - t1 == T2i(2, 4, 5, 4), "t2 - t1 failed");

  static_assert(Z * t2 == Z, "Z * t failed");
  static_assert(t2 * Z == Z, "t * Z failed");

  static_assert(E * t2 == t2, "E * t failed");
  static_assert(t2 * E == t2, "t * E failed");

  static_assert(t1 * t2 == T2i(3, 4, 10, 12), "t * t failed");
  static_assert(t2 * t1 == T2i(3, 8, 5, 12), "t * t failed");

  constexpr T2i t3(2);
  static_assert(E * 2 == T2i(2), "t * a failed");
  static_assert(t3 / 2 == T2i(1), "t / a failed");

  // vector ops
  constexpr V2i z(0), v(2, 3);
  static_assert(t2 * z == z, "t * z failed");
  static_assert(z * t2 == z, "z * t failed");
  static_assert(E * v == v, "E * v failed");
  static_assert(v * E == v, "v * E failed");
  static_assert(v * t2 == V2i(21, 26), "v * t failed");
  static_assert(t2 * v == V2i(18, 28), "t * v failed");
  static_assert((v ^ v) == T2i(4, 6, 6, 9), "v ^ v failed");

  // 2D vector ops
  static_assert(Z % v == V2i(0), "Z % v failed in 2D");
  static_assert(t2 % V2i(0) == V2i(0), "T % z failed in 2D");
  static_assert(t2 % v == V2i(1, 3), "t % v failed in 2D");
  static_assert(v % t3 == ~t3 % (-v), "v % t failed in 2D");

  // 3D vector ops
  constexpr T3i T(1, 2, 3, 4, 5, 6, 7, 8, 9);
  constexpr V3i zz(0), v1(1, 2, 3), v2(4, 5, 6);
  static_assert(~v1 * v2 == v1 % v2, "~v failed in 3D");
  static_assert(T3i(0) % v1 == T3i(0), "Z % v failed in 3D");
  static_assert(T % zz == T3i(0), "t % z failed in 3D");
  static_assert(T % v1 == T3i(0, 0, 0, 3, -6, 3, 6, -12, 6), "t % v failed in 3D");
  static_assert(v1 % T == ~(~T % (-v1)), "v % t failed in 3D");

  // other methods
  static_assert(E.det() == 1, "|E| failed");
  static_assert(Z.det() == 0, "|Z| failed ");
  static_assert(t1.det() == 2, "|t| failed");
  static_assert(t1.trace() == 3, "tr(t) failed");
  static_assert(E.invert() == E, "E^-1 failed");
  static_assert(E == ~E, "E^T failed");

  constexpr T2i t(2, 1, 3, 2);
  static_assert(~t == T2i(2, 3, 1, 2), "t^T failed");
  static_assert(t.invert() == T2i(2, -1, -3, 2), "t^-1 failed");
  static_assert(t.invert() * t == E, "t^-1 * t failed");
  static_assert(t * t.invert() == E, "t * t^-1 failed");
} // namespace Math::Tensors::tests

/*---------------------------------------------------------------------------------------*/
/*----------------------------------- documentation -------------------------------------*/
/*---------------------------------------------------------------------------------------*/

/*!
  \class Math::Tensor
  \brief Tensor of rank 2.
  \tparam N Spatial dimension (total number of tensor components is N*N).
  \tparam T Type of the components.

  It's quite simple Type To represent tensors of rank 2 / square matricies.
  A lot of useful methods and operations are defined to make it simple and intuitive
  similar to fundumental ones like int, double, etc. All constructors of the class
  defined as explicit in order to prevent reluctant conversions. It may be inconvenient
  sometimes, especially in generic code, but make the class safer for the user.

  \see Math::Vector
*/

/*!
  \fn constexpr explicit Tensor::Tensor(const Ts&... as) noexcept
  \brief Tensor initialization.
  \tparam as Pack of the initial values

  Four variants of initialization are possible depends on the number of parameters:
  - no parameters: all components are set to zero;
  - one parameter: diagonal components are set to the given value, rest are zeroes;
  - N parameters: diagonal components are set to the given values, rest are zeroes;
  - N*N parameters: all components are set to the given values, row by row.

  Only these combinations are possible, any other leads to a compilation error
  message about ambiguous number of the constructor arguments.

  \code
  using T2i = Tensor<2, int>;
  T2i td;             // [0, 0, 0, 0] ~ [0 0]
                      //                [0 0]

  T2i t1(1);          // [1, 0, 0, 1] ~ [1 0]
                      //                [0 1]

  T2i t2(1, 2);       // [1, 0, 0, 2] ~ [1 0]
                      //                [0 2]

  T2i t3(1, 2, 3, 4); // [1, 2, 3, 4] ~ [1 2]
                      //                [3 4]
  \endcode

  Note that all constructors are explicit thus you should be specific in any operation
  with tensors!
  */

/*!
  \fn constexpr explicit Tensor::Tensor(const Tensor &t) noexcept
  \brief Conversion constructor from a tensor with arbitrary type of the components.
  \tparam U Type of the given tensor' components.
  \param t Tensor to be converted.

  If the specified type U leads to a narrowing conversion then there won't be any warnings.
  In other words, I suppose you know what you do.
*/

/*!
  \fn constexpr Tensor& Tensor::operator=(const Tensor &t) noexcept
  \brief Conversion assignment from a tensor with arbitrary type of the components.
  \tparam U Type of the given tensor' components.
  \param t Tensor to be converted.
*/

/*!
  \property static constexpr size_t Tensor::dim
  \brief Spatial dimension (size of vector field).
*/

/*!
  \fn constexpr T* Tensor::operator[](size_t i) noexcept
  \brief Returns the specified row of a tensor.
  \param i Row number.
  \return Specified row of a tensor.
*/

/*!
  \fn constexpr const T* Tensor::operator[](size_t i) const noexcept
  \param i Row number.
  \return Specified row of a tensor.
  \brief Returns the specified row of a tensor. Constant version.
*/

/*!
  \fn constexpr Tensor Tensor::operator-() const noexcept
  \brief Unary minus.
  \return A copy of the given tensor with all its components multiplied by -1.
*/

/*!
  \fn constexpr Tensor Tensor::operator+() const noexcept
  \brief Unary plus.
  \return A copy of the given tensor.
*/

/*!
  \fn constexpr Tensor operator~() const noexcept
  \brief Transposition.
  \return Copy of the given tensor with flipped components values, A[i][j] == A[j][i]
*/

/*!
  \fn constexpr Tensor& Tensor::operator*=(const T &a) noexcept
  \brief Mutliplication assignment by scalar.
  \param a Factor, a value of the same type as tensor components (T).
  \return The tensor with all its components multiplied by "a" value.
*/

/*!
  \fn constexpr Tensor& Tensor::operator/=(const T &a) noexcept
  \brief Division assignment by scalar.
  \param a Divider, a value of the same type as tensor components (T).
  \return The tensor with all its components divided by "a" value.
*/

/*!
  \fn constexpr Tensor& Tensor::operator+=(const Tensor &A) noexcept
  \brief Addition assignment with tensor.
  \param A Addhend, a tensor of the same size (N) and type (T) of the components.
  \return The sum of the given tensor with the addhend A.
*/

/*!
  \fn constexpr Tensor& Tensor::operator-=(const Tensor &A) noexcept
  \brief Substruction assignment with tensor.
  \param A Substrahend, a tensor of the same size (N) and type (T) of the components.
  \return The difference of the given tensor and the substrahend A.
*/

/*!
  \fn constexpr Tensor& Tensor::operator*=(const Tensor &A) noexcept
  \brief Multiplication assignment by tensor.
  \param A Factor, a tensor of the same size (N) and type (T) of the components.
  \return The multiplication of the given tensor and the factor A.
*/

/*!
  \fn constexpr Tensor& Tensor::operator/=(const Tensor &A) noexcept
  \brief A shortcut for multiplication assignment by inverse tensor, A /= B == A *= B.inverse()
  \param A Factor, a tensor of the same size (N) and type (T) of the components.
  \return The multiplication of the given tensor and the inverse factor A.
*/

/*!
  \fn constexpr T Tensor::det() const noexcept
  \brief Compute the tensor' determinant.
  \return Determinant of the given tensor.
*/

/*!
  \fn constexpr T Tensor::trace() const noexcept
  \brief Compute the tensor' trace.
  \return Trace of the given tensor.
*/

/*!
  \fn constexpr Tensor Tensor::invert() const noexcept
  \brief Get the inverse tensor.
  \return The inverse tensor to the given one.
*/

/*!
  \fn constexpr Tensor Tensor::transpose() const noexcept
  \brief Get the transposed tensor.
  \return Tensor "B" with flipped components, i.e. B[i][j] == A[j][i]
  \see Tensor::operator~()
*/

/*!
  \fn constexpr auto operator+(Tensor A, const Tensor &B) noexcept
  \brief Component-wise addition of two tensors with the same dimension and type.
  \param A Left term, augend.
  \param B Right term, addhend.
  \return Tensor "C" with components C[i][j] == A[i][j] + B[i][j]
*/

/*!
  \fn constexpr auto operator-(Tensor A, const Tensor &B) noexcept
  \brief Component-wise substruction of two tensors with the same dimension and type.
  \param A Left term, minuend.
  \param B Right term, substrahend.
  \return Tensor "C" with components C[i][j] == A[i][j] - B[i][j]
*/

/*!
  \fn constexpr auto operator*(Tensor A, const Tensor &B) noexcept
  \brief Matrix multiplication of two tensors with the same dimension and type.
  \param A Left term, multiplicand.
  \param B Right term, multiplier.
  \return Tensor "C" with components C[i][j] == A[i][k] + B[k][j] for k = 0..(N-1)
*/

/*!
  \fn constexpr auto operator*(Tensor A, const T &a) noexcept
  \brief Component-wise tensor multiplication by scalar on the right.
  \param A Left term, tensor multiplicand.
  \param a Right term, scalar multiplier.
  \return Tensor "C" with components C[i][j] == A[i][j] * a
*/

/*!
  \fn constexpr auto operator*(const T &a, Tensor A) noexcept
  \brief Component-wise tensor multiplication by scalar on the left.
  \param a Left term, scalar multiplier.
  \param A Right term, tensor multiplicand.
  \return Tensor "C" with components C[i][j] == a * A[i][j]
*/

/*!
  \fn constexpr auto operator/(Tensor A, const T &a) noexcept
  \brief Component-wise tensor division by scalar.
  \param A Left term, tensor divident.
  \param a Right term, scalar divider.
  \return Tensor "C" with components C[i][j] == A[i][j] / a
*/

/*!
  \fn constexpr auto operator/(Tensor A, const Tensor &B) noexcept
  \brief A shortcut for tensor multiplication by inverse one, A/B == A*B.inverse()
  \param A Left term, divident.
  \param B Right term, divider.
  \return The multiplication of the left term by the inverse right term.
*/

/*!
  \fn constexpr bool operator==(const Tensor &A, const Tensor &B) noexcept
  \brief Component-wise equality comparison of two tensors.
  \param A Left term.
  \param B Right term.
  \return true if all components of the left term equals to the corresponding components
    of the right term, i.e. A[i][j] == B[i][j] for all i,j = 0..(N-1)
*/

/*!
  \fn constexpr bool operator!=(const Tensor &A, const Tensor &B) noexcept
  \brief Component-wise inequality comparison of two tensors.
  \param A Left term.
  \param B Right term.
  \return true if any component of the left term not equal to the corresponding component
    of the right term, i.e. exists i, j: A[i][j] != B[i][j]
*/

/*!
  \fn std::istream& operator>>(std::istream &in, Tensor &A)
  \brief Read a tensor from input stream.
  \param in Input stream from which tensor is read.
  \param A Tensor where read object will be stored.
  \return Input stream after reading the tensor from it.
  \see IOMode

  Tensor may be represented in two formats -- w/ and w/o brackets,
  e.g. "[1, 2, 3, 4]" and "1 2 3 4" both represent the same tensor.
  This function is able to read in both formats, none additional actions needed
  in order to successfully read a tensor.
*/

/*!
  \fn std::ostream& operator<<(std::ostream &out, const Tensor &A)
  \brief Write a tensor to output stream.
  \param out Output stream where tensor is written.
  \param A Tensor which will be written to the output stream.
  \return Output stream after writing the tensor to it.
  \see IOMode

  Tensor may be represented in two formats -- w/ and w/o brackets, e.g.
  "[1, 2, 3, 4]" and "1 2 3 4" both represent the same tensor.
  You can choose the format using special manipulators inBrackets() and bareComps(),
  the default is inBrackets. The second one may be useful for e.g. backup purposes.
*/

/*!
  \fn constexpr auto& operator*=(Vector &a, const Tensor &A) noexcept
  \brief Post-multiplication with assignment of vector by tensor.
  \param a Vector multiplicand.
  \param A Tensor multiplier.
  \return Vector as a result of post-multiplication of the given vector by given tensor.
*/

/*!
  \fn constexpr auto operator*(Vector a, const Tensor &A) noexcept
  \brief Post-multiplication of vector by tensor.
  \param a Vector multiplicand.
  \param A Tensor multiplier.
  \return Vector as a result of post-multiplication of the given vector by given tensor.
*/

/*!
  \fn constexpr auto operator*(const Tensor &A, Vector a) noexcept
  \brief Pre-multiplication of vector by tensor (matrix).
  \param A Tensor multiplicand.
  \param a Vector multiplier.
  \return Vector as a result of pre-multiplication of the given vector by given tensor.
*/

/*!
  \fn constexpr auto& operator/=(Vector &a, const Tensor &A) noexcept
  \brief A shortcut for post-multiplication with assignment of vector by inverse tensor, a /= A == a *= A.inverse()
  \param a Vector divident.
  \param A Tensor divider.
  \return Vector as a result of post-multiplication of the given vector by inverse given tensor.
*/

/*!
  \fn constexpr auto operator/(Vector a, const Tensor &A) noexcept
  \brief A shortcut for post-multiplication of vector by inverse tensor, a / A == a * A.inverse()
  \param a Vector divident.
  \param A Tensor divider.
  \return Vector as a result of post-multiplication of the given vector by inverse given tensor.
*/

/*!
  \fn constexpr auto operator^(const Vector &a, const Vector &b) noexcept
  \brief Diadic product of two vectors, e.g. for two 2D vectors:
    a^b == (ax, ay) ^ (bx, by) = [ax*bx, ax*by, ay*bx, ay*by]
  \param a First vector.
  \param b Second one.
  \return Tensor as a result of diadic product of the two given vectors.
*/

/*!
  \fn constexpr auto operator%(const Tensor &A, const Vector &a) noexcept
  \brief Vector product of tensor by vector in 2D, i.e. A % a == (Axx*ay - Axy*ax, Ayx*ay - Ayy*ax)
  \param A 2D tensor, multiplicand
  \param a 2D vector, multiplier
  \return Vector as a result of vector product of the given tensor by vector in 2D.
*/

/*!
  \fn constexpr auto operator%(const Vector &a, const Tensor &A) noexcept
  \brief Vector product of vector by tensor in 2D, i.e. a % A == (ax*Ayx - ay*Axx, ax*Ayy - ay*Axy)
  \param a 2D vector, multiplicand
  \param A 2D tensor, multiplier
  \return Vector as a result of vector product of the given vector by tensor in 2D.
*/

/*!
  \fn constexpr auto operator%(const Tensor &A, const Tensor &B) noexcept
  \brief Vector product of two tensors in 2D
  \code
  A % B == [Axx * Byx - Axy * Bxx    Axx * Byy - Axy * Bxy]
  [Ayx * Byx - Ayy * Bxx    Ayx * Byy - Ayy * Bxy]
  \endcode
  \param A 2D tensor, multiplicand
  \param B 2D tensor, multiplier
  \return Tensor as a result of vector product of two tensors in 2D.
*/

/*!
  \fn constexpr Tensor<3, T> operator~(const Vector &a) noexcept
  \brief Useful operation to represent vector product of two vectors in tensor form, i.e.
  for two vectors "a" and "b", ~a*b = a%b. From the formula
  \code
  ~a == [ 0  -az   ay]
  [ az  0   -ax]
  [-ay  ax   0 ]
  \endcode
  \param a 3D vector
  \return Tensor which scalar multiplication on a vector is equivalent to the
  vector product of the original vector to it.
*/

/*!
  \fn constexpr auto operator%(const Tensor &A, const Vector &a) noexcept
  \brief Vector product of tensor by vector in 3D, i.e.
  \code
  A % a ==
  [Axy * az - Axz * ay   Axz * ax - Axx * az   Axx * ay - Axy * ax]
  [Ayy * az - Ayz * ay   Ayz * ax - Ayx * az   Ayx * ay - Ayy * ax]
  [Azy * az - Azz * ay   Azz * ax - Azx * az   Azx * ay - Azy * ax]
  \endcode
  \param A 3D tensor, multiplicand
  \param a 3D vector, multiplier
  \return Tensor as a result of vector product of the given tensor by vector in 3D.
*/

/*!
  \fn constexpr auto operator%(const Vector &a, const Tensor &A) noexcept
  \brief Vector product of vector by tensor in 3D, i.e.
  \code
  a % A ==
  [ay * Azx - az * Ayx   ay * Azy - az * Ayy   ay * Azz - az * Ayz]
  [az * Axx - ax * Azx   az * Axy - ax * Azy   az * Axz - ax * Azz]
  [ax * Ayx - ay * Axx   ax * Ayy - ay * Axy   ax * Ayz - ay * Axz]
  \endcode
  \param a 3D vector, multiplicand
  \param A 3D tensor, multiplier
  \return Tensor as a result of vector product of the given vector by tensor in 3D.
*/

#endif // MATH_TENSOR_H_INCLUDED
