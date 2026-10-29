<div align="center">

<img src="img/logo.png" alt="IDA Logo" width="350">

# Information Dispersal Algorithm

**SPLIT A SENTENCE INTO N FRAGMENTS, REBUILD IT FROM ANY M OF THEM!**

[![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org)
[![CMake](https://img.shields.io/badge/CMake-3.26+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org)
[![Docker](https://img.shields.io/badge/Docker-Ready-2496ED?style=for-the-badge&logo=docker&logoColor=white)](#docker)
[![Verify](https://img.shields.io/badge/Verify-clang--tidy_%2B_sanitizers-D9534F?style=for-the-badge&logo=llvm&logoColor=white)](#verification)

A C++17 implementation of Rabin's Information Dispersal Algorithm (IDA), with an
interactive console that shows every step of the encoding and the decoding.

</div>

---

## Contents

- [Overview](#overview)
- [Commands](#commands)
- [Features](#features)
  - [Encoding and decoding](#encoding-and-decoding)
  - [Finite fields](#finite-fields)
  - [Output](#output)
  - [Characters](#characters)
  - [Input checks](#input-checks)
- [Architecture](#architecture)
- [Tech Stack](#tech-stack)
- [Setup](#setup)
- [Docker](#docker)
- [Parameters](#parameters)
- [Testing](#testing)
- [Verification](#verification)
- [Documentation](#documentation)
- [Limitations](#limitations)
- [Reference](#reference)
- [Author](#author)

---

## Overview

IDA splits data into **n** fragments so that **any m** of them (m < n) are
enough to rebuild the original: up to n − m fragments can be lost and the data
can still be recovered.

The interactive console `ida` encodes the sentences you type into n fragments,
rebuilds them from the m fragments you choose and, with `set detail=all`,
prints every intermediate matrix. The arithmetic is done in one of two finite
fields, chosen with `set field`:

- **GF(2^8)**, the Galois field with 256 elements. Every encoded value is a
  byte, so fragments can be written to a file as they are.
- **GF(257)**, the integers modulo the prime 257. Encoded values range from
  0 to 256: the value 256 does not fit in a byte, so fragments cannot be
  written to a file one byte per value.

---

## Commands

| Command | Description |
| :--- | :--- |
| `set n=<value>` | Number of fragments generated |
| `set m=<value>` | Number of fragments needed to rebuild the sentence |
| `set field=<galois\|prime>` | Field used for the arithmetic: GF(2^8) or GF(257) |
| `set fragments=<i,j,...>` | The m fragments used to rebuild the sentence |
| `set detail=<results\|all>` | Only the results, or every intermediate step too |
| `help` | Shows the help again, with the current value of every parameter |
| `exit` | Closes the console, as Ctrl-D does |
| *any other line* | A sentence, encoded and rebuilt with the current parameters |

Defaults and allowed values are in [Parameters](#parameters).

---

## Features

### Encoding and decoding

Each sentence goes through these steps:

1. **Character codes** — each character becomes one value, its code from 0
   to 255 (see [Characters](#characters))
2. **Text matrix** — the codes are laid out column by column in a matrix B
   with m rows and ⌈L / m⌉ columns, where L is the length of the sentence;
   null characters are appended as padding when L is not a multiple of m
3. **Coding matrix** — A is an n × m Vandermonde matrix: row i is
   (1, i+1, (i+1)², …, (i+1)^(m−1)), with the powers computed in the field, so
   any m of its rows are linearly independent
4. **Encoding** — C = A · B: each row of C is a fragment
5. **Inverse** — the rows of A that match the chosen fragments form a square
   matrix A′, inverted with **Gauss-Jordan elimination**: row operations in the
   field turn the augmented matrix [A′ | I] into [I | A′⁻¹], in O(m³)
   operations
6. **Decoding** — B = A′⁻¹ · C′, where C′ holds the chosen fragments; removing
   the padding gives back the sentence

Constraints:
- 1 ≤ m < n < number of elements of the field: n is at most **255** in GF(2^8)
  and **256** in GF(257)
- Exactly **m** distinct fragments, each an index from 0 to n − 1

### Finite fields

A field is a set where addition, subtraction, multiplication and division by
a non-zero element always give an element of the set. The algorithm only needs
these four operations, so it works the same way in both fields:

| Operation | GF(257) | GF(2^8) |
| :--- | :--- | :--- |
| Addition | (a + b) mod 257 | a XOR b |
| Subtraction | (a − b) mod 257 | a XOR b |
| Multiplication | (a · b) mod 257 | polynomial product mod x⁸ + x⁴ + x³ + x + 1 (AES) |
| Inverse | a^255 | a^254 |

- In GF(2^8) each byte is read as a polynomial of degree below 8 with
  coefficients 0 or 1: bit k is the coefficient of x^k
- In both fields the inverse comes from the same rule: in a field with q
  elements, a^(q−1) = 1 for every non-zero a, so a⁻¹ = a^(q−2)

### Output

`ida` starts with a help that lists every parameter with its current value,
then shows the prompt `$> `; a blank line separates each output from the next
prompt.

```
$> set n=6

$> set m=3
The fragments no longer fit n and m: fragments=0,1,2

$> Il barone Fanfulla da Lodi!

Data: Il barone Fanfulla da Lodi!

Encoded matrix:
  5 113 100   7 125  97  37   3  44
 17 115  60  51 109  43 119  31  50
 93  96  55  20 126  38 114  60 122
212 156 198 105 252 141 225 161 253
152 143 205  78 239 128 228 130 181
140 141 149 122 255 202 182 158 171

Decoded matrix:
 73  98 111  32 110 108  32  32 100
108  97 110  70 102 108 100  76 105
 32 114 101  97 117  97  97 111  33

Rebuild data:
Il barone Fanfulla da Lodi!

$> costa 5€
Error: The character "€" at position 8 has the code 8364, outside the range 0-255
```

- **`detail=results`** — the sentence, the encoded matrix, the decoded matrix
  and the rebuilt sentence
- **`detail=all`** — also the character codes, every product of the encoding,
  the coding, text, inverse and fragment matrices
- **Aligned matrices** — the values are right-aligned in columns; a matrix
  wider than 80 characters is split into blocks of columns, each with a header
  such as `(columns 1-20 of 58)` and separated by a blank line

### Characters

The console reads UTF-8 text and gives each character **one value**, its
code, so that the IDA encodes one value per character:

- Characters with a code from **0 to 255** are accepted: ASCII and the
  accented letters of Latin-1, such as è (232), à and ü
- Characters with a larger code, such as "€" (8364) and emojis, are rejected
  with their position, and nothing is encoded
- The rebuilt sentence is converted back to UTF-8

### Input checks

Every choice that cannot rebuild the sentence is rejected with a message, and
the console goes on with the next line. With n = 4 and m = 2:

| Choice | Message |
| :--- | :--- |
| `fragments=0,1,2` (three fragments, m = 2) | `There must be exactly m labels, one for each column of the coding matrix!` |
| `fragments=0,4` (fragment 4 does not exist) | `Fragment label out of range!` |
| `fragments=-1,2` | `Fragment label out of range!` |
| `fragments=1,1` (the same fragment twice) | `The selected matrix is not invertible!` |
| `m=4` (m equal to n) | `The parameter m must be strictly less than n` |
| `m=0` | `The coding matrix must have at least one row and one column` |
| `n=256` in GF(2^8), `n=257` in GF(257) | `The parameter n must be less than the number of elements of the field` |
| `m=abc` | `"abc" is not an integer` |
| `colour=red` | `Unknown parameter "colour": type help to see the parameters` |

---

## Architecture

```
InformationDispersalAlgorithm/
├── CMakeLists.txt                  # Library, console and tests
├── CMakePresets.json               # The verify preset (full verification)
├── Dockerfile                      # Builds and tests the program, then runs the console
├── img/
│   └── logo.png                    # Logo of the project
├── cmake/
│   └── QualityChecks.cmake         # Compiler warnings and on-request checks
├── src/
│   ├── main.cpp                    # Reads the lines typed in the console, shows the errors
│   ├── Field/
│   │   ├── Field.h / .cpp          # Abstract base class: add, subtract, multiply, power, inverse
│   │   ├── PrimeField.h / .cpp     # Prime field GF(p)
│   │   └── GaloisField256.h / .cpp # Galois field GF(2^8)
│   ├── Matrix/
│   │   ├── Matrix.h / .cpp         # Base class: values, size, selection of rows
│   │   ├── TextMatrix.h / .cpp     # Text to encode (B)
│   │   ├── CodingMatrix.h / .cpp   # Vandermonde coding matrix (A)
│   │   ├── EncodedMatrix.h / .cpp  # Encoded fragments (C)
│   │   └── InverseCodingMatrix.h / .cpp  # Inverse of the coding submatrix (A′⁻¹), Gauss-Jordan
│   ├── Ida/
│   │   ├── IDA.h / .cpp            # Encoding and decoding
│   │   ├── EncodingStep.h          # How one value of C is computed
│   │   └── FieldProduct.h          # One product of the encoding
│   └── Console/
│       ├── Console.h / .cpp        # Commands and sentences typed in the console
│       ├── Settings.h              # Parameters of the console
│       └── Latin1Converter.h / .cpp  # UTF-8 text to one code from 0 to 255 per character
└── Tests/
    ├── TestUtil.h                  # check() and report(), shared by the tests
    ├── FieldTest.cpp               # Arithmetic of both fields
    ├── RoundtripTest.cpp           # Encoding and decoding with both fields
    ├── InvalidInputTest.cpp        # Invalid inputs are rejected with an exception
    ├── InvalidParameterTest.cpp    # Invalid parameters are rejected with an exception
    ├── IntermediateResultsTest.cpp # Intermediate results exposed by IDA
    ├── Latin1ConverterTest.cpp     # Conversion of the characters typed in the console
    ├── ConsoleTest.cpp             # Commands and sentences of the console
    ├── CheckProgramOutput.cmake    # Runs the program on an input file and compares its output
    ├── ProgramInput.txt            # Lines typed into the console by ProgramOutputTest
    └── ExpectedOutput.txt          # What the console prints for those lines
```

- Each folder under `src/` is a module, and each module only depends on the
  ones before it: `Field` → `Matrix` → `Ida` → `Console` → `main.cpp`
- Every include starts from `src/`, for example `#include "Field/Field.h"`, and
  all the code is in the namespace `ida`
- `IDA`, `CodingMatrix` and `InverseCodingMatrix` receive a `Field` and do
  every operation through it: adding a new field only takes a new subclass of
  `Field`

### Output and errors

The classes of `Field`, `Matrix` and `Ida` only compute: none of them prints.
`Console` turns the results into the text to show and, with the detailed
output, the intermediate ones, which `IDA` exposes:

- `computeEncodingSteps()` returns, for each value of C, the products
  A[i][k] · B[k][j] and their sum
- `computeDecodedValues()` returns the decoded matrix, padding included

An invalid command, parameter or input throws `std::invalid_argument` or
`std::out_of_range` with a message. `main` catches it, prints `Error: ` and
the message on standard error, and the console goes on with the next line.

### Memory management

No object is freed by hand: every allocation is owned by something that frees
it automatically.

| Kind | Used for |
| :--- | :--- |
| **Local objects** | Objects that do not leave their function, such as `A`, `B`, `dispersal` and `inverse` in `Console::encodeAndDecode()`: no allocation, destroyed at the end of the block, used with the dot notation. `encode()` and `selectFragments()` also build their result as a local object and move it into a `std::unique_ptr` only when they return |
| **`std::unique_ptr`** | Objects with a single owner: `encode()` and `selectFragments()` return one, so the caller owns the new matrix |
| **`std::shared_ptr<const Field>`** | The field, used by the coding matrix, its inverse and the `IDA` object at the same time, and never changed |
| **`std::vector`** | The values of the matrices (`MatrixData`) |
| **References** | Parameters used without owning them, such as the matrices passed to `encode()` and `decode()` (`const T &` when only read). A reference cannot be null, so no null checks are needed; `encode()` and `selectFragments()` never return a null pointer, and the `InverseCodingMatrix` constructor never leaves a half-built object: they throw an exception when they cannot compute their result |
| **By value** | Small types (`int`, `long`), and parameters the function stores, followed by `std::move` (the text in `TextMatrix`, the field in the constructors, the values of the matrices) |
| **`const std::vector<int> &`** | The fragment indices, so that every function knows how many there are |

---

## Tech Stack

The program uses only the C++ standard library.

### Build and run

| Tool | Role |
| :--- | :--- |
| **C++17 compiler** (GCC or Clang) | Compiles the library, the console and the tests |
| **CMake 3.26+** | Build, presets and test registration (CTest) |
| **Docker** (optional) | Builds, tests and runs the console in an Ubuntu 24.04 container |

### Development tools

| Tool | Role |
| :--- | :--- |
| **CTest** | Runs the test programs |
| **clang-format** | Formatting, configured in `.clang-format` |
| **clang-tidy** | Static analysis, configured in `.clang-tidy` (verify preset) |
| **AddressSanitizer + UndefinedBehaviorSanitizer** | Memory errors and undefined behavior while the tests run (verify preset) |
| **Doxygen** | HTML documentation from the comments |

---

## Setup

> **Prerequisites:** Git, CMake 3.26+, a C++17 compiler (GCC or Clang).

**1. Clone the repository**

```bash
git clone git@github.com:sean8819/InformationDispersalAlgorithm.git
cd InformationDispersalAlgorithm
```

**2. Build**

```bash
cmake -S . -B build
cmake --build build
```

**3. Run the tests**

```bash
ctest --test-dir build
```

**4. Start the console**

```bash
./build/ida
```

The compiler warnings are always on.

---

## Docker

Docker needs nothing else installed: the image compiles the program and runs
every test while it is built, so it is produced only if all the tests pass.

```bash
docker build -t ida .
docker run --rm -it ida
```

`-it` connects the console to your terminal: without it the console gets no
input and closes at once. After changing the sources, build the image again.

---

## Parameters

| Parameter | Default | Allowed values |
| :--- | :---: | :--- |
| `n` | `4` | m < n < number of elements of the field: at most 255 in GF(2^8), 256 in GF(257) |
| `m` | `2` | 1 ≤ m < n |
| `field` | `galois` | `galois` for GF(2^8), `prime` for GF(257) |
| `fragments` | `0,3` | m distinct indices from 0 to n − 1, separated by commas |
| `detail` | `results` | `results` or `all` |

- Spaces around `=` and after the commas are allowed
- `set` checks only the form of a value; whether the parameters fit together
  is checked when a sentence is run
- When n or m change and the fragments no longer fit, they go back to the
  first m, and the console says so

---

## Testing

The project has **8 CTest tests**; RoundtripTest alone performs 11220
decodings.

| Test | Focus |
| :--- | :--- |
| `FieldTest` | Arithmetic of both fields: GF(2^8) against the examples of the AES standard (FIPS-197), a · a⁻¹ = 1 for every non-zero element |
| `RoundtripTest` | Five texts, one with every byte value from 1 to 255, with both fields and (n, m) = (3, 1), (4, 2), (6, 3), (10, 4), (12, 5), (14, 12), decoded with every possible choice of m fragments |
| `InvalidInputTest` | Fragment indices outside the matrix, a number of indices different from m, repeated indices, matrices with another field or the wrong size, a negative padding |
| `InvalidParameterTest` | Invalid constructor parameters (m = 0, a field with fewer than 256 elements, a modulus that is not prime) and their messages |
| `IntermediateResultsTest` | Products and sums of the encoding, decoded matrix and padding exposed by `IDA` |
| `Latin1ConverterTest` | Every code from 0 to 255 converted to UTF-8 and back; "€", emojis and byte sequences that are not UTF-8 rejected |
| `ConsoleTest` | Commands, sentences rebuilt with the default and the changed parameters, rejected sentences, aligned matrices and blocks of columns |
| `ProgramOutputTest` | Types `Tests/ProgramInput.txt` into the console and checks that it prints exactly `Tests/ExpectedOutput.txt`, nothing on standard error, with exit code 0 |

**Run the tests:**

```bash
ctest --test-dir build
```

---

## Verification

The full verification is one command, run by hand when a change is complete:

```bash
cmake --workflow --preset verify --fresh
```

It configures a clean build in `build-verify/`, builds everything and runs all
the tests with:

- **Warnings as errors** — any compiler warning stops the build
- **clang-tidy** — its findings stop the build too
- **AddressSanitizer + UndefinedBehaviorSanitizer** — out-of-bounds accesses,
  use after free, leaks and undefined behavior are reported while the tests
  run

---

## Documentation

The sources are documented with Doxygen comments. To generate the HTML
documentation, create a configuration with `doxygen -g`, then set:

```
RECURSIVE              = YES
EXCLUDE                = build build-verify cmake-build-debug
USE_MDFILE_AS_MAINPAGE = README.md
```

Then run `doxygen`. The output is written to `html/`.

---

## Limitations

This code is meant to explain how the algorithm works, so it does not follow
every good software engineering practice. In particular:

- Powers and the inverses of the field elements are computed in the
  simplest way, by repeated multiplication, instead of with logarithm tables
  or the extended Euclidean algorithm.

The integrity of the fragments is outside the scope of this project: the
fragments used for decoding are assumed to be the ones produced by the
encoding. A fragment altered in storage or in transit is not detected, and
the decoding gives a wrong text without any error; only in GF(257) a decoded
value outside 0 ... 255 is sometimes reported as an error. Rabin's paper
suggests adding a fingerprint to the fragments to recognize the altered ones.

---

## Reference

M. O. Rabin, "Efficient dispersal of information for security, load balancing,
and fault tolerance", *Journal of the ACM*, 36(2):335–348, 1989.

---

## Author

|  | Name | GitHub |
| :--- | :--- | :--- |
| Author | Sean8819 | [@sean8819](https://github.com/sean8819) |
