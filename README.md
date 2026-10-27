# Information Dispersal Algorithm

A C++17 implementation of Rabin's Information Dispersal Algorithm (IDA).
IDA splits data into **n** fragments so that **any m** of them (m < n) are
enough to rebuild the original. Up to n − m fragments can be lost and the data
can still be recovered.

The project shows the encoding and decoding steps in detail: with verbose mode
on, it prints every intermediate matrix. It comes in two versions, which differ
only in the finite field used for the arithmetic:

- **GF(2^8)**, the Galois field with 256 elements. Every encoded value is a
  byte, so fragments can be written to a file as they are.
- **GF(257)**, the integers modulo the prime 257. Encoded values range from
  0 to 256: the value 256 does not fit in a byte, so fragments cannot be
  written to a file one byte per value.

## How it works

A field is a set where addition, subtraction, multiplication and division by
a non-zero element always give an element of the set. The algorithm only needs
these four operations, so it works the same way in both fields:

| Operation      | GF(257)                 | GF(2^8)                                            |
|----------------|-------------------------|----------------------------------------------------|
| Addition       | (a + b) mod 257         | a XOR b                                            |
| Subtraction    | (a − b) mod 257         | a XOR b                                            |
| Multiplication | (a · b) mod 257         | polynomial product mod x⁸ + x⁴ + x³ + x + 1 (AES)  |
| Inverse        | a^255                   | a^254                                              |

In GF(2^8) each byte is read as a polynomial of degree below 8 with
coefficients 0 or 1: bit k is the coefficient of x^k. In both fields the
inverse comes from the same rule: in a field with q elements, a^(q−1) = 1 for
every non-zero a, so a⁻¹ = a^(q−2).

1. **Text matrix.** The text is split into m rows and ⌈L / m⌉ columns, where L
   is the text length. Characters are laid out column by column, and null
   characters are appended as padding when L is not a multiple of m. This
   gives the matrix B (m × ⌈L / m⌉).
2. **Coding matrix.** A is an n × m Vandermonde matrix: row i is
   (1, i+1, (i+1)², …, (i+1)^(m−1)), with the powers computed in the field.
   Any m of its rows are linearly independent.
3. **Encoding.** C = A · B. Each row of C is a fragment.
4. **Decoding.** Pick any m fragments. The matching rows of A form a square
   matrix A′, which is invertible. Its inverse is computed with the adjugate
   matrix and the inverse of the determinant. Then B = A′⁻¹ · C′, where C′
   holds the selected fragments. Removing the padding gives back the original
   text.

## Project structure

```
├── CMakeLists.txt
├── README.md
├── src
│   ├── main.cpp                      Usage example
│   ├── Field
│   │   ├── Field.h / .cpp            Abstract base class: add, sub, mul, pow, inv
│   │   ├── PrimeField.h / .cpp       Prime field GF(p)
│   │   └── GaloisField256.h / .cpp   Galois field GF(2^8)
│   ├── Matrix
│   │   ├── Matrix.h / .cpp           Abstract base class
│   │   ├── TextMatrix.h / .cpp       Text to encode (B)
│   │   ├── CodingMatrix.h / .cpp     Vandermonde coding matrix (A)
│   │   ├── EncodedMatrix.h / .cpp    Encoded fragments (C)
│   │   └── InverseCodingMatrix.h / .cpp   Inverse of the coding submatrix (A′⁻¹)
│   └── Ida
│       └── IDA.h / .cpp              Encoding and decoding
└── Tests
    ├── TestUtil.h                    check() and report(), shared by the tests
    ├── FieldTest.cpp                 Arithmetic of both fields
    ├── RoundtripTest.cpp             Encoding and decoding with both fields
    ├── InvalidInputTest.cpp          Invalid inputs are rejected safely
    ├── InvalidParameterTest.cpp      Invalid parameters stop the program
    └── OutputTest.cpp                What is printed with and without verbose
```

Each folder under `src/` is a module, and each module only depends on the ones
before it: `Field` → `Matrix` → `Ida` → `main.cpp`. Every include starts from
`src/`, for example `#include "Field/Field.h"`.

`IDA`, `CodingMatrix` and `InverseCodingMatrix` receive a `Field` and do every
operation through it. Adding a new field only takes a new subclass of `Field`.

## Memory management

No object is freed by hand: every allocation is owned by something that frees
it automatically.

- **Local objects** whenever an object does not leave its function, such as
  `A`, `B` and `ida` in `main.cpp`. They need no allocation, they are
  destroyed at the end of the block, and they are used with the dot notation.
  `encode()`, `getFragments()` and `getInverse()` also build their result as
  a local object and move it into a `std::unique_ptr` only when they return.
- **`std::unique_ptr`** for objects with a single owner. `encode()`,
  `getFragments()` and `getInverse()` return one, so the caller owns the new
  matrix.
- **`std::shared_ptr`** for the field, which is used by the coding matrix, its
  inverse and the `IDA` object at the same time.
- **`std::vector`** for the values of the matrices (`MatrixData`).
- **References** for parameters that use an object without owning it, such
  as the matrices passed to `encode()` and `decode()`: `const T &` when the
  function only reads the object. A reference cannot be null, so these
  functions do not need null checks; the caller checks the pointers returned
  by `encode()` and `getInverse()` before using them.
- **By value** for small types (`int`, `long`, `bool`), and for parameters the
  function stores, followed by `std::move` (the text in `TextMatrix`, the
  field in the constructors).
- **`const std::vector<int> &`** for the fragment indices, so that every
  function knows how many there are.

## Build and run

You need CMake 3.26 or later and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
./build/InformationDispersalAlgorithm
```

## Tests

```bash
ctest --test-dir build
```

- **FieldTest** checks the arithmetic of both fields: the GF(2^8) results
  against the examples of the AES standard (FIPS-197), and a · a⁻¹ = 1 for
  every non-zero element of both fields.
- **RoundtripTest** encodes five texts, one of them with every byte value
  from 1 to 255, with both fields and (n, m) = (3, 1), (4, 2), (6, 3),
  (10, 4), (12, 5). It decodes each one with every possible choice of m
  fragments and checks that the original text comes back.
- **InvalidInputTest** checks that invalid inputs are rejected with an error
  instead of causing a memory error or a wrong result: fragment indices outside
  the matrix, a number of indices different from m, matrices with another
  field or the wrong size, and labels different from those used to compute
  the inverse.
- **InvalidParameterTest** passes invalid parameters to the constructors
  (for example m = 0, a field with fewer than 256 elements, a modulus that is
  not prime) and checks that the program stops with the right error message.
  Each case is a separate CTest test.
- **OutputTest** checks that, without verbose, only the encoded and decoded
  matrices are printed, and that verbose adds every intermediate step.

## Configuration

The parameters are set in `src/main.cpp`:

| Variable      | Default        | Meaning                                              |
|---------------|----------------|------------------------------------------------------|
| `n`           | 4              | Number of fragments generated                        |
| `m`           | 2              | Number of fragments needed for reconstruction        |
| `p`           | 257            | Prime modulus, used when `galoisField` is `false`    |
| `verbose`     | `false`        | Also print every intermediate step                   |
| `galoisField` | `true`         | Use GF(2^8) if `true`, GF(p) if `false`              |
| `data`        | sample string  | Text to encode                                       |
| `labels`      | `{0, 3}`       | Fragments used for decoding (`std::vector` of m indices) |

The parameters must satisfy 1 ≤ m < n < number of elements of the field, so
n is at most 255 for GF(2^8) and p − 1 for GF(p). p must be a prime of at
least 257, so that every byte value is an element of the field. Otherwise the
program stops with an error message.

Without verbose, the program prints the text, the encoded matrix, the decoded
matrix and the rebuilt text. With verbose, it also prints the character codes,
every product of the encoding, the coding, text, inverse and fragment
matrices.

## Documentation

The sources are documented with Doxygen comments. To generate the HTML
documentation, create a configuration with `doxygen -g`, then set:

```
RECURSIVE              = YES
EXCLUDE                = build cmake-build-debug
USE_MDFILE_AS_MAINPAGE = README.md
```

Then run `doxygen`. The output is written to `html/`.

## Limitations

This code is meant to explain how the algorithm works, so it does not follow
every good software engineering practice. In particular:

- Powers, inverses and determinants are computed in the simplest way
  (repeated multiplication and Laplace expansion), which is slow for large
  values of m.

## Reference

M. O. Rabin, "Efficient dispersal of information for security, load balancing,
and fault tolerance", *Journal of the ACM*, 36(2):335–348, 1989.

## Author

Sean8819
