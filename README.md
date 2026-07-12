# numkit

A C++ header-only library providing low-level mathematical data structures for numerical computations.

## Core Components

### Type (`math/Type.h`)

A C++20 concept constraining types eligible for numerical computations:

- Requires default and copy constructibility
- Requires compound assignment operators (`+=`, `-=`, `*=`, `/=`) and equality (`==`)
- Intentionally broader than `std::is_arithmetic` to support user-defined numeric types

### Vector (`math/Vector.h`)

A templated Euclidean vector class for arbitrary dimensions and `Math::Type`-constrained types:

- Supports standard arithmetic operations (`+`, `-`, `*`, `/`)
- Provides dot product, cross product (`%`), magnitude (`fabs`), and angle functions (`cos`, `sin`)
- Type aliases: `Vector2D`, `Vector3D`, and `Array<N,T>` for simple componentwise arithmetic

### Tensor (`math/Tensor.h`)

A rank-2 tensor (matrix) class for arbitrary dimensions and `Math::Type`-constrained types:

- Full matrix operations: transpose, inverse, determinant, trace
- Matrix multiplication, addition, subtraction
- Vector-tensor operations for linear algebra
- Type aliases: `Tensor2D`, `Tensor3D`

### State (`quantities/State.h`)

A heterogeneous tuple of named quantities for scientific state vectors:

- Uses `Traits` for type metadata with compile-time string names
- Component types are constrained by `Math::Type`
- Supports arithmetic and comparison operations on states
- Allows access by index, type-name, or variable
- Useful for representing physical states (e.g., density, temperature, velocity)

### ObjectsFactory (`factory/ObjectsFactory.h`)

A generic factory pattern implementation:

- Creates objects by string identifiers
- Uses static registration via constructors
- Supports arbitrary creation functions with typed arguments

### Common library

Common utilities and helpers:

- IOMode provides io manipulators `inBrackets` and `bareComponents` and functions for range-like types

## Project Structure

```text
numkit/
├── CMakeLists.txt       # Main build config
├── common/              # Common library
│   └── common/          # IOMode
├── math/                # Math library
│   └── math/            # Type, Vector, Tensor
├── quantities/          # Quantities library
│   └── quantities/      # State, Traits
├── factory/             # Factory pattern implementation
│   └── factory/         # ObjectsFactory
└── tests/               # Unit tests
    ├── tst_vector.cpp
    ├── tst_tensor.cpp
    ├── tst_state.cpp
    ├── tst_factory.cpp
    └── ...
```

## Build

Requires CMake 3.23+ and a C++20 compatible compiler. Right now there is nothing to build, it's a header-only library.

### Tests

Requires [Google Test](https://github.com/google/googletest).

```bash
cmake -B build -DNUMKIT_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

### Documentation

Requires [Doxygen](https://www.doxygen.nl/).

```bash
cmake -B build -DNUMKIT_BUILD_DOCS=ON
cmake --build build --target doc
```

The generated HTML documentation will be available in `build/docs/html/`.

## Known Issues / TODOs

- No examples :(
- Materials with deducing this 8) (c++23)
- Tensor with multidimentional subscript op (c++23)
- units of quantities
- states made with traits-variables, not types
- ...
