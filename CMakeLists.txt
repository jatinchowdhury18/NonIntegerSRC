cmake_minimum_required(VERSION 3.1)
project(NonIntegerSRC)
set(CMAKE_CXX_STANDARD 17)

set(BUILD_TESTING OFF CACHE BOOL "Don't build CMake tests!")
add_subdirectory(third_party/libsamplerate)

add_executable(src_test)

target_sources(src_test PRIVATE
    src/main.cpp
)

find_package(Python3 COMPONENTS Development NumPy)

target_include_directories(src_test PRIVATE
    third_party/libsamplerate/include
    ${Python3_INCLUDE_DIRS}
    ${Python3_NumPy_INCLUDE_DIRS}
)

target_link_libraries(src_test PUBLIC
    samplerate
    Python3::Python
    Python3::NumPy
)
