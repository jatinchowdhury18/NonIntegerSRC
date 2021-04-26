cmake_minimum_required(VERSION 3.1)
project(NonIntegerSRC)
set(CMAKE_CXX_STANDARD 17)


add_executable(src_test)

target_sources(src_test PRIVATE
  src/main.cpp
)

# Link to libsamplerate
set(BUILD_TESTING OFF CACHE BOOL "Don't build CMake tests!")
add_subdirectory(third_party/libsamplerate)

target_include_directories(src_test PRIVATE third_party/libsamplerate/include)
target_link_libraries(src_test PUBLIC samplerate)

# use matplotlibcpp (if needed)
set(USEMATPLOTLIB OFF)
if(USEMATPLOTLIB)
  find_package(Python3 COMPONENTS Development NumPy)
  
  target_include_directories(src_test PRIVATE
      ${Python3_INCLUDE_DIRS}
      ${Python3_NumPy_INCLUDE_DIRS}
      third_party
  )
  
  target_link_libraries(src_test PUBLIC
      Python3::Python
      Python3::NumPy
  )

  target_compile_definitions(src_test PUBLIC MAKE_PLOTS=1)
endif()
