cmake_minimum_required(VERSION 3.20...3.30)

project(Auctoritas
    VERSION 0.0.1
    DESCRIPTION "A massive space sim"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds are forbidden. Please create a build directory and run cmake from there.")
endif()

set(BINARY_NAME "Auctoritas")
add_executable(${BINARY_NAME}
    src/main.cpp
    src/utils.cpp
)

target_include_directories(${BINARY_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(${BINARY_NAME} PRIVATE /W4 /WX /permissive-)
else()
    target_compile_options(${BINARY_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
