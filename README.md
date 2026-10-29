# Information Dispersal Algorithm

A C++17 implementation of Rabin's Information Dispersal Algorithm (IDA).
IDA splits data into **n** fragments so that **any m** of them (m < n) are
enough to rebuild the original. Up to n − m fragments can be lost and the data
can still be recovered.

The project shows the encoding and decoding steps in detail: an interactive
console encodes and rebuilds the sentences you type and, with
`set detail=all`, prints every intermediate matrix. It comes in two versions,
which differ only in the finite field used for the arithmetic:

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
   matrix A′, which is invertible. Its inverse is computed with Gauss-Jordan
   elimination: row operations in the field turn the augmented matrix
   [A′ | I] into [I | A′⁻¹], in O(m³) operations. Then B = A′⁻¹ · C′, where
   C′ holds the selected fragments. Removing the padding gives back the
   original text.

## Project structure

```
├── CMakeLists.txt
├── CMakePresets.json                 The verify preset (full verification)
├── Dockerfile                        Builds and tests the program, then runs it in a container
├── cmake
│   └── QualityChecks.cmake           Compiler warnings and on-request checks
├── README.md
├── src
│   ├── main.cpp                      Reads the lines typed in the console, shows the errors
│   ├── Field
│   │   ├── Field.h / .cpp            Abstract base class: add, subtract, multiply,
│   │   │                             computePower, computeInverse
│   │   ├── PrimeField.h / .cpp       Prime field GF(p)
│   │   └── GaloisField256.h / .cpp   Galois field GF(2^8)
│   ├── Matrix
│   │   ├── Matrix.h / .cpp           Base class: values, size, selection of rows
│   │   ├── TextMatrix.h / .cpp       Text to encode (B)
│   │   ├── CodingMatrix.h / .cpp     Vandermonde coding matrix (A)
│   │   ├── EncodedMatrix.h / .cpp    Encoded fragments (C)
│   │   └── InverseCodingMatrix.h / .cpp   Inverse of the coding submatrix (A′⁻¹)
│   ├── Ida
│   │   ├── IDA.h / .cpp              Encoding and decoding
│   │   ├── EncodingStep.h            How one value of C is computed
│   │   └── FieldProduct.h            One product of the encoding
│   └── Console
│       ├── Console.h / .cpp          Commands and sentences typed in the console
│       ├── Settings.h                Parameters of the console
│       └── Latin1Converter.h / .cpp  UTF-8 text to one code from 0 to 255 per character
└── Tests
    ├── TestUtil.h                    check() and report(), shared by the tests
    ├── FieldTest.cpp                 Arithmetic of both fields
    ├── RoundtripTest.cpp             Encoding and decoding with both fields
    ├── InvalidInputTest.cpp          Invalid inputs are rejected with an exception
    ├── InvalidParameterTest.cpp      Invalid parameters are rejected with an exception
    ├── IntermediateResultsTest.cpp   Intermediate results exposed by IDA
    ├── Latin1ConverterTest.cpp       Conversion of the characters typed in the console
    ├── ConsoleTest.cpp               Commands and sentences of the console
    ├── CheckProgramOutput.cmake      Runs the program on an input file and compares its output
    ├── ProgramInput.txt              Lines typed into the console by ProgramOutputTest
    └── ExpectedOutput.txt            What the console prints for those lines
```

Each folder under `src/` is a module, and each module only depends on the ones
before it: `Field` → `Matrix` → `Ida` → `Console` → `main.cpp`. Every include starts from
`src/`, for example `#include "Field/Field.h"`. All the code is in the
namespace `ida`.

`IDA`, `CodingMatrix` and `InverseCodingMatrix` receive a `Field` and do every
operation through it. Adding a new field only takes a new subclass of `Field`.

## Output and errors

The classes of `Field`, `Matrix` and `Ida` only compute: none of them prints.
`Console` turns the results into the text to show and, with the detailed
output, the intermediate ones, which `IDA` exposes:

- `computeEncodingSteps()` returns, for each value of C, the products
  A[i][k] · B[k][j] and their sum;
- `computeDecodedValues()` returns the decoded matrix, padding included.

An invalid command, parameter or input (for example m = 0, a fragment index
outside the matrix, a character with a code above 255) throws
`std::invalid_argument` or `std::out_of_range` with a message. `main` catches
it, prints `Error: ` and the message on standard error, and the console goes
on with the next line.

## Memory management

No object is freed by hand: every allocation is owned by something that frees
it automatically.

- **Local objects** whenever an object does not leave its function, such as
  `A`, `B`, `dispersal` and `inverse` in `Console::encodeAndDecode()`. They need no allocation,
  they are destroyed at the end of the block, and they are used with the dot
  notation. `encode()` and `selectFragments()` also build their result as a
  local object and move it into a `std::unique_ptr` only when they return.
- **`std::unique_ptr`** for objects with a single owner. `encode()` and
  `selectFragments()` return one, so the caller owns the new matrix.
- **`std::shared_ptr<const Field>`** for the field, which is used by the
  coding matrix, its inverse and the `IDA` object at the same time, and never
  changes.
- **`std::vector`** for the values of the matrices (`MatrixData`).
- **References** for parameters that use an object without owning it, such
  as the matrices passed to `encode()` and `decode()`: `const T &` when the
  function only reads the object. A reference cannot be null, so these
  functions do not need null checks; `encode()` and `selectFragments()` never
  return a null pointer, and the `InverseCodingMatrix` constructor never leaves
  a half-built object: they throw an exception when they cannot compute their
  result.
- **By value** for small types (`int`, `long`), and for parameters the
  function stores, followed by `std::move` (the text in `TextMatrix`, the
  field in the constructors, the values of the matrices).
- **`const std::vector<int> &`** for the fragment indices, so that every
  function knows how many there are.

## Build and run

You need CMake 3.26 or later and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
./build/ida
```

The compiler warnings are always on. The full verification (warnings as
errors, clang-tidy, AddressSanitizer and UndefinedBehaviorSanitizer) is one
command, to run when a change is complete:

```bash
cmake --workflow --preset verify --fresh
```

## Run with Docker

Docker needs nothing else installed: the image compiles the program and runs
every test while it is built, so it is produced only if all the tests pass.

```bash
docker build -t ida .
docker run --rm -it ida
```

`-it` connects the console to your terminal: without it the console gets no
input and closes at once.

## Tests

```bash
ctest --test-dir build
```

- **FieldTest** checks the arithmetic of both fields: the GF(2^8) results
  against the examples of the AES standard (FIPS-197), and a · a⁻¹ = 1 for
  every non-zero element of both fields.
- **RoundtripTest** encodes five texts, one of them with every byte value
  from 1 to 255, with both fields and (n, m) = (3, 1), (4, 2), (6, 3),
  (10, 4), (12, 5), (14, 12). It decodes each one with every possible choice of m
  fragments and checks that the original text comes back.
- **InvalidInputTest** checks that invalid inputs are rejected with an
  exception instead of causing a memory error or a wrong result: fragment
  indices outside the matrix, a number of indices different from m, repeated
  indices, matrices with another field or the wrong size, and a negative
  padding.
- **InvalidParameterTest** passes invalid parameters to the constructors
  (for example m = 0, a field with fewer than 256 elements, a modulus that is
  not prime) and checks that they throw `std::invalid_argument` with the
  right message.
- **IntermediateResultsTest** checks the intermediate results that `IDA`
  exposes: the products and sums of the encoding, the decoded matrix and the
  padding.
- **Latin1ConverterTest** checks that every character with a code from 0 to
  255 becomes that single code and back, and that "€", emojis and byte
  sequences that are not UTF-8 are rejected.
- **ConsoleTest** checks the commands (help, set with every parameter, wrong
  values), the sentences rebuilt with the default and the changed parameters,
  and the sentences rejected because of a character or a parameter.
- **ProgramOutputTest** types the lines of `Tests/ProgramInput.txt` into the
  console (the sample sentence with the default parameters) and checks that
  it prints exactly `Tests/ExpectedOutput.txt`, nothing on standard error, and
  ends with exit code 0.

## Using the console

`ida` starts with a help that lists every parameter with its current value,
then shows the prompt `$> `. Each line is a command or a sentence:

| Line                | Effect                                                                  |
|---------------------|-------------------------------------------------------------------------|
| `set n=6`           | Number of fragments generated (default 4)                               |
| `set m=3`           | Number of fragments needed for reconstruction (default 2)               |
| `set field=prime`   | `galois` for GF(2^8) (default), `prime` for GF(257)                     |
| `set fragments=0,3` | The m fragments used for decoding (default 0,3)                         |
| `set detail=all`    | `results` (default) or `all`, which also prints every intermediate step |
| `help`              | Shows the help again, with the current values                           |
| `exit`              | Closes the console, as Ctrl-D does                                      |
| any other line      | A sentence, encoded and rebuilt with the current parameters             |

```
$> set n=6

$> set m=3
The fragments no longer fit n and m: fragments=0,1,2

$> Perché però è così?

Data: Perché però è così?

Encoded matrix:
 71 226  53 160 171 240  63
 73  58  79  13  63  20  63
...

Rebuild data:
Perché però è così?

$> costa 5€
Error: The character "€" at position 8 has the code 8364, outside the range 0-255
```

Spaces around `=` and after the commas are allowed. When n or m change and
the fragments no longer fit (m indices from 0 to n − 1), they go back to the
first m, and the console says so.

`set` checks only the form of a value. Whether the parameters fit together is
checked when a sentence is run: they must satisfy 1 ≤ m < n < number of
elements of the field, so n is at most 255 for GF(2^8) and 256 for GF(257).

With `detail=results`, the console prints the text, the encoded matrix, the
decoded matrix and the rebuilt text. With `detail=all`, it also prints the
character codes, every product of the encoding, the coding, text, inverse and
fragment matrices. The values of a matrix are right-aligned in columns; a
matrix wider than 80 characters is split into blocks of columns, each with a
header such as `(columns 1-20 of 58)`.

The console reads UTF-8 text. Each character becomes one value, its code, so
the characters must have a code from 0 to 255: ASCII and the accented letters
of Latin-1, such as è, à and ü, are accepted, while "€" and emojis are
rejected with their position. The rebuilt text is converted back to UTF-8.

## Documentation

The sources are documented with Doxygen comments. To generate the HTML
documentation, create a configuration with `doxygen -g`, then set:

```
RECURSIVE              = YES
EXCLUDE                = build build-verify cmake-build-debug
USE_MDFILE_AS_MAINPAGE = README.md
```

Then run `doxygen`. The output is written to `html/`.

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

## Reference

M. O. Rabin, "Efficient dispersal of information for security, load balancing,
and fault tolerance", *Journal of the ACM*, 36(2):335–348, 1989.

## Author

Sean8819
