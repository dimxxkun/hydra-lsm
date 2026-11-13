cmake_minimum_required(VERSION 3.15)
project(hydra_lsm VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(hydra_lsm ${SOURCES})

if (MSVC)
    target_compile_options(hydra_lsm PRIVATE /W4)
else()
    target_compile_options(hydra_lsm PRIVATE -Wall -Wextra -pedantic -O2)
endif()
