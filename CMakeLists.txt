cmake_minimum_required(VERSION 3.1)
project(NonIntegerSRC)

add_subdirectory(third_party/libsamplerate)

add_executable(src_test)

target_sources(src_test PRIVATE
    src/main.cpp
)

target_include_directories(src_test PUBLIC third_party/libsamplerate/include)
target_link_libraries(src_test PUBLIC samplerate)
