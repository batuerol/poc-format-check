# CMakeList.txt : CMake project for compile-time-format-str-check, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (poc-format-check "main.cpp")

#set(CMAKE_CXX_FLAGS "-Wformat=2")
set (CMAKE_CXX_STANDARD 14)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET poc-format-check PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
