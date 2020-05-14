cmake_minimum_required(VERSION 3.17)
project(Compiler)

set(CMAKE_CXX_COMPILER g++-9)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-I/usr/local/opt/flex/include")

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

# Sources

include_directories(include)

file(GLOB HEADERS include/*.hpp)
file(GLOB SOURCES src/*.cpp)

add_executable(
        interpreter
        interpreter.cpp
        ${HEADERS}
        ${SOURCES}
        ${BISON_parser_OUTPUTS}
        ${FLEX_scanner_OUTPUTS})

#llvm_map_components_to_libnames(llvm_libs support core irreader)
#
## Link against LLVM libraries
#target_link_libraries(compiler ${llvm_libs})