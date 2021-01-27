cmake_minimum_required(VERSION 3.17)
project(Compiler)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-I/usr/local/opt/flex/include")
set(CMAKE_CXX_FLAGS "-Wall -O3 -march=native")

# FLEX and BISON

find_package(BISON 3.6 REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(parser utils/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
FLEX_TARGET(scanner utils/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
ADD_FLEX_BISON_DEPENDENCY(scanner parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# LLVM

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Common sources

include_directories(include)

file(GLOB HEADERS include/*.hpp)
file(GLOB SOURCES src/*.cpp)

# Interpreter

add_executable(
        interpreter
        interpreter.cpp
        ${HEADERS}
        ${SOURCES}
        ${BISON_parser_OUTPUTS}
        ${FLEX_scanner_OUTPUTS})

# Compiler

add_executable(
        compiler
        compiler.cpp
        ${HEADERS}
        ${SOURCES}
        ${BISON_parser_OUTPUTS}
        ${FLEX_scanner_OUTPUTS})

llvm_map_components_to_libnames(llvm_libs support core passes x86asmparser x86codegen)

# Link against LLVM libraries
target_link_libraries(compiler ${llvm_libs})
target_link_libraries(interpreter ${llvm_libs}) # too lazy to not GLOB sources