cmake_minimum_required(VERSION 3.26)
project(InformationDispersalAlgorithm CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Warnings and on-request checks: it must come before any target.
include(cmake/QualityChecks.cmake)

#The logic is a library, so the program and the tests use the same code.
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
        src/Matrix/VandermondeMatrix.cpp
        src/Matrix/VandermondeMatrix.h
        src/Matrix/EncodedMatrix.cpp
        src/Matrix/EncodedMatrix.h
        src/Matrix/InverseCodingMatrix.cpp
        src/Matrix/InverseCodingMatrix.h
        src/Ida/FieldProduct.h
        src/Ida/EncodingStep.h
        src/Ida/IDA.cpp
        src/Ida/IDA.h
        src/Console/Settings.h
        src/Console/Latin1Converter.cpp
        src/Console/Latin1Converter.h
        src/Console/Console.cpp
        src/Console/Console.h
)
#Every include starts from src/, e.g. #include "Field/Field.h".
target_include_directories(IDALibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(ida src/main.cpp)
target_link_libraries(ida IDALibrary)

#Tests: run them with "ctest" from the build directory.
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

add_executable(IntermediateResultsTest Tests/IntermediateResultsTest.cpp)
target_link_libraries(IntermediateResultsTest IDALibrary)
add_test(NAME IntermediateResultsTest COMMAND IntermediateResultsTest)

add_executable(InvalidParameterTest Tests/InvalidParameterTest.cpp)
target_link_libraries(InvalidParameterTest IDALibrary)
add_test(NAME InvalidParameterTest COMMAND InvalidParameterTest)

add_executable(Latin1ConverterTest Tests/Latin1ConverterTest.cpp)
target_link_libraries(Latin1ConverterTest IDALibrary)
add_test(NAME Latin1ConverterTest COMMAND Latin1ConverterTest)

add_executable(ConsoleTest Tests/ConsoleTest.cpp)
target_link_libraries(ConsoleTest IDALibrary)
add_test(NAME ConsoleTest COMMAND ConsoleTest)

#The console, fed with the lines of Tests/ProgramInput.txt, must print exactly Tests/ExpectedOutput.txt.
add_test(NAME ProgramOutputTest
        COMMAND ${CMAKE_COMMAND} -DPROGRAM=$<TARGET_FILE:ida>
                -DPROGRAM_INPUT=${CMAKE_CURRENT_SOURCE_DIR}/Tests/ProgramInput.txt
                -DEXPECTED_OUTPUT=${CMAKE_CURRENT_SOURCE_DIR}/Tests/ExpectedOutput.txt
                -P ${CMAKE_CURRENT_SOURCE_DIR}/Tests/CheckProgramOutput.cmake)
