#include "math/Tensor.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace Math;

using T2d = Tensor<2>;
using T3d = Tensor<3>;
using T2i = Tensor<2, int>;
using T3i = Tensor<3, int>;

using V2i = Vector<2, int>;
using V3i = Vector<3, int>;

TEST(Tensor, copy_constructor)
{
  T3i t1(1);
  T3i t1c(t1);
  EXPECT_EQ(t1c, t1);
}

TEST(Tensor, copy_assignment)
{
  T3i t1(1, 2, 3);
  T3i t1c = t1;
  EXPECT_EQ(t1c, t1);
}

TEST(Tensor, brace_copy)
{
  T3i t1{1, 2, 3, 4, 5, 6, 7, 8, 9};
  T3i t1c{t1};
  EXPECT_EQ(t1c, t1);
}

TEST(Tensor, equality)
{
  T3i t1(1), t2(1);
  EXPECT_EQ(t1, t2);
}

TEST(Tensor, inequality)
{
  T3i t1(1), t2(2);
  EXPECT_NE(t1, t2);
}

TEST(Tensor, addition)
{
  T3i t0(0), t1(1), t2(2);
  EXPECT_EQ(t0 + t1, t1);
  EXPECT_EQ(t1 + t0, t1);
  EXPECT_EQ(t1 + t1, t2);
}

TEST(Tensor, subtraction)
{
  T3i t0(0), t1(1);
  EXPECT_EQ(t1 - t0, t1);
  EXPECT_EQ(t0 - t1, -t1);
}

TEST(Tensor, scalar_multiplication)
{
  T3i t1(1), t2(2);
  EXPECT_EQ(2 * t1, t2);
}

TEST(Tensor, scalar_division)
{
  T3i t1(1), t2(2);
  EXPECT_EQ(t2 / 2, t1);
}

TEST(Tensor, unary_minus)
{
  T3i t1(1);
  EXPECT_EQ(-(-t1), t1);
}

TEST(Tensor, tensor_times_vector)
{
  T2i t(3, 4, 5, 6);
  V2i v(2, 3);
  EXPECT_EQ(t * v, V2i(18, 28));
}

TEST(Tensor, vector_times_tensor)
{
  T2i t(3, 4, 5, 6);
  V2i v(2, 3);
  EXPECT_EQ(v * t, V2i(21, 26));
}

TEST(Tensor, identity_times_vector)
{
  T3i E(1, 1, 1);
  V3i v(1, 2, 3);
  EXPECT_EQ(E * v, v);
  EXPECT_EQ(v * E, v);
}

TEST(Tensor, assign_add)
{
  T3i t(1);
  t += t;
  EXPECT_EQ(t, T3i(2));
}

TEST(Tensor, assign_sub)
{
  T3i t(2);
  t -= T3i(1);
  EXPECT_EQ(t, T3i(1));
}

TEST(Tensor, assign_mul)
{
  T3i t(1);
  t *= 2;
  EXPECT_EQ(t, T3i(2));
}

TEST(Tensor, assign_div)
{
  T3i t(2);
  t /= 2;
  EXPECT_EQ(t, T3i(1));
}

TEST(Tensor, transpose)
{
  T3i t(1, 2, 3, 4, 5, 6, 7, 8, 9);
  T3i tT(1, 4, 7, 2, 5, 8, 3, 6, 9);
  EXPECT_EQ(~t, tT);
  EXPECT_EQ(t.transpose(), ~t);
}

TEST(Tensor, transpose_identity)
{
  T3i E(1, 1, 1);
  EXPECT_EQ(~E, E);
}

TEST(Tensor, trace)
{
  T3i t(1, 2, 3, 0, 1, 4, 5, 6, 0);
  EXPECT_EQ(t.trace(), 2);
}

TEST(Tensor, determinant_identity)
{
  T3i E(1, 1, 1);
  EXPECT_EQ(E.det(), 1);
}

TEST(Tensor, determinant_zero)
{
  T3i Z(0);
  EXPECT_EQ(Z.det(), 0);
}

TEST(Tensor, determinant_3x3)
{
  T3i t(1, 2, 3, 0, 1, 4, 5, 6, 0);
  EXPECT_EQ(t.det(), 1);
}

TEST(Tensor, determinant_4x4)
{
  using T4i = Tensor<4, int>;
  T4i t4(2, 3, 5, 2,
         6, 1, 8, 3,
         5, 4, 9, 2,
         1, 3, 5, 6);
  EXPECT_EQ(t4.det(), -1);
}

TEST(Tensor, invert_identity)
{
  T3i E(1, 1, 1);
  EXPECT_EQ(E.invert(), E);
}

TEST(Tensor, invert_3x3)
{
  T3i E(1, 1, 1);
  T3i t(1, 2, 3, 0, 1, 4, 5, 6, 0);
  T3i R(-24, 18, 5, 20, -15, -4, -5, 4, 1);
  EXPECT_EQ(t.invert(), R);
  EXPECT_EQ(t.invert() * t, E);
  EXPECT_EQ(t * t.invert(), E);
}

TEST(Tensor, invert_4x4)
{
  using T4i = Tensor<4, int>;
  T4i E4(1, 1, 1, 1);
  T4i t4(2, 3, 5, 2,
         6, 1, 8, 3,
         5, 4, 9, 2,
         1, 3, 5, 6);
  T4i t4_inverted(
      121,  28, -76, -29,
       88,  20, -55, -21,
     -113, -26,  71,  27,
       30,   7, -19,  -7);
  EXPECT_EQ(t4.invert(), t4_inverted);
  EXPECT_EQ(t4 * t4_inverted, E4);
}

TEST(Tensor, output_default_in_brackets)
{
  std::stringstream ss;
  ss << T2i(1, 2, 3, 4);
  EXPECT_EQ(ss.str(), "[1, 2, 3, 4]");
}

TEST(Tensor, output_in_brackets)
{
  std::stringstream ss;
  ss << IO::inBrackets << T2i(1, 2, 3, 4);
  EXPECT_EQ(ss.str(), "[1, 2, 3, 4]");
}

TEST(Tensor, output_in_brackets_signed)
{
  std::stringstream ss;
  ss << IO::inBrackets << T2i(-1, +2, -3, +4);
  EXPECT_EQ(ss.str(), "[-1, 2, -3, 4]");
}

TEST(Tensor, output_bare_comps)
{
  std::stringstream ss;
  ss << IO::bareComps << T2i(1, 2, 3, 4);
  EXPECT_EQ(ss.str(), "1 2 3 4");
}

TEST(Tensor, input_in_brackets)
{
  std::stringstream ss("[1, 2, 3, 4]");
  T2i t;
  ss >> t;
  EXPECT_EQ(t, T2i(1, 2, 3, 4));
}

TEST(Tensor, input_in_brackets_signed)
{
  std::stringstream ss("[-1, 2, -3, 4]");
  T2i t;
  ss >> t;
  EXPECT_EQ(t, T2i(-1, 2, -3, 4));
}

TEST(Tensor, input_in_brackets_missing_closing_bracket)
{
  T2i t;
  std::stringstream ss("[1, 2, 3, 4");
  EXPECT_TRUE(!(ss >> t));
}

TEST(Tensor, input_in_brackets_extra_comms)
{
  T2i t;
  std::stringstream ss("[1, 2, 3, 4,]");
  EXPECT_TRUE(!(ss >> t));
}

TEST(Tensor, input_in_brackets_missing_value)
{
  T2i t;
  std::stringstream ss("[1, 2, 3]");
  EXPECT_TRUE(!(ss >> t));
}

TEST(Tensor, input_in_brackets_extra_value)
{
  T2i t;
  std::stringstream ss("[1, 2, 3, 4, 5]");
  EXPECT_TRUE(!(ss >> t));
}

TEST(Tensor, input_bare_comps)
{
  std::stringstream ss("1 2 3 4");
  T2i t;
  ss >> t;
  EXPECT_EQ(t, T2i(1, 2, 3, 4));
}

TEST(Tensor, input_bare_comps_signed)
{
  std::stringstream ss("-1 2 -3 4");
  T2i t;
  ss >> t;
  EXPECT_EQ(t, T2i(-1, 2, -3, 4));
}

TEST(Tensor, input_bare_comps_missing_values)
{
  T2i t;
  std::stringstream ss("1 2 3");
  EXPECT_TRUE(!(ss >> t));
}

TEST(Tensor, roundtrip_in_brackets)
{
  T2i t1(1, -2, 3, -4), t2;
  std::stringstream ss;
  ss << IO::inBrackets << t1;
  ss >> t2;
  EXPECT_EQ(t1, t2);
}

TEST(Tensor, roundtrip_bare_comps)
{
  T2i t1(5, 6, 7, 8), t2;
  std::stringstream ss;
  ss << IO::bareComps << t1;
  ss >> t2;
  EXPECT_EQ(t1, t2);
}
