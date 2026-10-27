cmake_minimum_required(VERSION 3.26)
project(InformationDispersalAlgorithm)

set(CMAKE_CXX_STANDARD 17)

# Shared code, used by the program and by the tests.
add_library(IDALibrary STATIC
        src/Field/Field.cpp
        src/Field/Field.h
        src/Field/PrimeField.cpp
        src/Field/PrimeField.h
        src/Field/GaloisField256.cpp
        src/Field/GaloisField256.h
        src/Matrix/Matrix.cpp
        src/Matrix/Matrix.h
        src/Matrix/TextMatrix.cpp
        src/Matrix/TextMatrix.h
        src/Matrix/CodingMatrix.cpp
        src/Matrix/CodingMatrix.h
        src/Matrix/EncodedMatrix.cpp
        src/Matrix/EncodedMatrix.h
        src/Matrix/InverseCodingMatrix.cpp
        src/Matrix/InverseCodingMatrix.h
        src/Ida/IDA.cpp
        src/Ida/IDA.h
)
# Every include starts from src/, e.g. #include "Field/Field.h".
target_include_directories(IDALibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(InformationDispersalAlgorithm src/main.cpp)
target_link_libraries(InformationDispersalAlgorithm IDALibrary)

# Tests: run them with "ctest" from the build directory.
enable_testing()

add_executable(FieldTest Tests/FieldTest.cpp)
target_link_libraries(FieldTest IDALibrary)
add_test(NAME FieldTest COMMAND FieldTest)

add_executable(RoundtripTest Tests/RoundtripTest.cpp)
target_link_libraries(RoundtripTest IDALibrary)
add_test(NAME RoundtripTest COMMAND RoundtripTest)

add_executable(InvalidInputTest Tests/InvalidInputTest.cpp)
target_link_libraries(InvalidInputTest IDALibrary)
add_test(NAME InvalidInputTest COMMAND InvalidInputTest)

add_executable(OutputTest Tests/OutputTest.cpp)
target_link_libraries(OutputTest IDALibrary)
add_test(NAME OutputTest COMMAND OutputTest)

# Each case passes an invalid parameter and must stop the program with the given message.
add_executable(InvalidParameterTest Tests/InvalidParameterTest.cpp)
target_link_libraries(InvalidParameterTest IDALibrary)

function(add_invalid_parameter_test testCase message)
    add_test(NAME InvalidParameterTest.${testCase} COMMAND InvalidParameterTest ${testCase})
    set_tests_properties(InvalidParameterTest.${testCase} PROPERTIES PASS_REGULAR_EXPRESSION "${message}")
endfunction()

add_invalid_parameter_test(null-field "The field cannot be null")
add_invalid_parameter_test(m-zero "The parameter m must be at least 1")
add_invalid_parameter_test(m-not-less-than-n "The parameter m must be strictly less than n")
add_invalid_parameter_test(n-too-large "The parameter n must be less than the number of elements of the field")
add_invalid_parameter_test(small-field "The field must have at least 256 elements")
add_invalid_parameter_test(not-prime "The modulus p must be a prime number")
add_invalid_parameter_test(zero-modulus "The modulus p must be a prime number")
add_invalid_parameter_test(coding-matrix-no-columns "The coding matrix must have at least one row and one column")
add_invalid_parameter_test(coding-matrix-null-field "The field cannot be null")
add_invalid_parameter_test(text-matrix-no-rows "The text matrix must have at least one row")
