cmake_minimum_required(VERSION 3.15)
project(pyregent LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find nanobind
find_package(Python 3.8 COMPONENTS Interpreter Development REQUIRED)
find_package(nanobind CONFIG REQUIRED)

# Disable benchmarks, tests, and parent Python build for this Python build
set(REGENT_BUILD_BENCHMARKS OFF CACHE BOOL "Disable benchmarks" FORCE)
set(REGENT_BUILD_TESTS OFF CACHE BOOL "Disable tests" FORCE)
set(REGENT_BUILD_EXAMPLES OFF CACHE BOOL "Disable examples" FORCE)
set(REGENT_BUILD_PYTHON OFF CACHE BOOL "Disable parent Python build" FORCE)

# Add the parent libregent as a subdirectory
add_subdirectory(.. libregent)

# Create the Python module
nanobind_add_module(
    pyregent
    NB_STATIC  # Build as static library
    pyregent.cpp
)

# Link against libregent
target_link_libraries(pyregent PRIVATE regent)
target_include_directories(pyregent PRIVATE ../include)

# Install the module
install(TARGETS pyregent LIBRARY DESTINATION .)
