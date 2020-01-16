cmake_minimum_required(VERSION 3.15)
project(compiler_containers)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts -fpermissive")
#link_libraries(iberty)

add_executable(compiler_containers main.cpp counter.h value_pair.h container.h constptr.h concept_types.h user_deduction.h demangle_names.h)