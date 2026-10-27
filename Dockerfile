#Build stage: compiles the program and runs every test, so the image is produced only if all the tests pass.
#Ubuntu 24.04 provides CMake 3.28: the project needs at least 3.26.
FROM ubuntu:24.04 AS build

RUN apt-get update \
    && apt-get install --yes --no-install-recommends g++ cmake make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /source

COPY CMakeLists.txt ./
COPY cmake cmake
COPY src src
COPY Tests Tests

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel \
    && ctest --test-dir build --output-on-failure

#Run stage: only the program, on the same base image so the C++ runtime library matches the compiler.
FROM ubuntu:24.04

COPY --from=build /source/build/InformationDispersalAlgorithm /usr/local/bin/ida

#The program needs no privileges.
RUN useradd --system --no-create-home ida
USER ida

ENTRYPOINT ["ida"]
