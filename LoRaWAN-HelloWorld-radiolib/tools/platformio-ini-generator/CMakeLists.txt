cmake_minimum_required(VERSION 3.20)

project(platformio_ini_generator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(snodec REQUIRED)

add_executable(platformio-ini-generator
    src/main.cpp
)

target_link_libraries(platformio-ini-generator
    PRIVATE
        snodec::snodec
)

target_compile_options(platformio-ini-generator
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
