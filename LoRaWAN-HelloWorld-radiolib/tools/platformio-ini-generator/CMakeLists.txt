cmake_minimum_required(VERSION 3.14)

project(platformio_ini_generator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(
    snodec 1.0.0 REQUIRED COMPONENTS
        http-server-express-legacy-in
)

add_executable(platformio-ini-generator
    src/main.cpp
)

target_link_libraries(platformio-ini-generator
    PRIVATE
        snodec::http-server-express-legacy-in
)

target_compile_options(platformio-ini-generator
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
