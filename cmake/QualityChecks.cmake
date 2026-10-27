# Compiler warnings and on-request quality checks: warnings as errors, clang-tidy and sanitizers.
# Include it from the top-level CMakeLists.txt right after project() and BEFORE any target is created:
#     include(cmake/QualityChecks.cmake)
# Warnings are always shown; the options turn on the stricter checks on request, e.g. cmake -DWARNINGS_AS_ERRORS=ON.

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(RUN_CLANG_TIDY "Run clang-tidy while compiling" OFF)
option(ENABLE_SANITIZERS "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(ENABLE_THREAD_SANITIZER "Build with ThreadSanitizer (cannot be combined with ENABLE_SANITIZERS)" OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wold-style-cast -Wnon-virtual-dtor
                    -Woverloaded-virtual -Wnull-dereference)

#Indices are signed int on purpose, so -Wsign-conversion is not used. GCC does not include it in -Wconversion
#for C++, but Clang (and clang-tidy, which parses the code with Clang) does: it is turned off explicitly.
add_compile_options(-Wno-sign-conversion)

#These warnings exist only in GCC.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wshadow=local -Wduplicated-cond -Wlogical-op -Wuseless-cast)
endif ()

if (WARNINGS_AS_ERRORS)
    set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif ()

if (RUN_CLANG_TIDY)
    find_program(CLANG_TIDY_PROGRAM clang-tidy REQUIRED)
    #clang-tidy parses the code with Clang, which does not know the GCC-only warning options above.
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_PROGRAM} --extra-arg=-Wno-unknown-warning-option)

    #With WARNINGS_AS_ERRORS, the findings of clang-tidy stop the build too.
    if (WARNINGS_AS_ERRORS)
        list(APPEND CMAKE_CXX_CLANG_TIDY --warnings-as-errors=*)
    endif ()
endif ()

if (ENABLE_SANITIZERS AND ENABLE_THREAD_SANITIZER)
    message(FATAL_ERROR "ThreadSanitizer cannot be combined with AddressSanitizer: enable only one of them")
endif ()

if (ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif ()

if (ENABLE_THREAD_SANITIZER)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
endif ()
