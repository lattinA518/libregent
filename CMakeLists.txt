cmake_minimum_required(VERSION 3.20)
project(libregent VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Release build if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Options
option(REGENT_BUILD_TESTS "Build tests" ON)
option(REGENT_BUILD_PYTHON "Build Python bindings" ON)
option(REGENT_BUILD_EXAMPLES "Build example programs" ON)
option(REGENT_WITH_UDPIPE "Build with UDPipe for self-contained parsing" OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Core library sources
set(REGENT_SOURCES
    src/types.cpp
    src/dep_graph.cpp
    src/rule.cpp
    src/rule_registry.cpp
    src/transformer.cpp
    src/ordering.cpp
    src/cue_words.cpp
    src/determiners.cpp
    src/referring.cpp
    src/anaphora.cpp
    src/lineariser.cpp
    src/ranker.cpp
    src/conllu.cpp
    src/regent.cpp
)

# Core library
add_library(regent ${REGENT_SOURCES})
target_include_directories(regent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Enable position-independent code for Python bindings
set_target_properties(regent PROPERTIES POSITION_INDEPENDENT_CODE ON)

# UDPipe integration (optional)
if(REGENT_WITH_UDPIPE)
    find_package(UDPipe)
    if(UDPipe_FOUND)
        target_sources(regent PRIVATE src/udpipe_adapter.cpp)
        target_link_libraries(regent PRIVATE udpipe)
        target_compile_definitions(regent PUBLIC REGENT_WITH_UDPIPE)
    else()
        message(WARNING "UDPipe not found, building without UDPipe support")
    endif()
endif()

# Python bindings
if(REGENT_BUILD_PYTHON)
    # Try to find nanobind
    find_package(Python 3.8 COMPONENTS Interpreter Development.Module REQUIRED)

    # Use FetchContent to get nanobind if not found
    include(FetchContent)
    FetchContent_Declare(
        nanobind
        GIT_REPOSITORY https://github.com/wjakob/nanobind.git
        GIT_TAG v2.0.0
    )
    FetchContent_MakeAvailable(nanobind)

    nanobind_add_module(_regent python/pyregent.cpp)
    target_link_libraries(_regent PRIVATE regent)

    install(TARGETS _regent LIBRARY DESTINATION .)
endif()

# CLI tool
if(REGENT_BUILD_EXAMPLES)
    add_executable(regent-cli examples/regent_cli.cpp)
    target_link_libraries(regent-cli PRIVATE regent)

    install(TARGETS regent-cli RUNTIME DESTINATION bin)
endif()

# Tests
if(REGENT_BUILD_TESTS)
    enable_testing()

    # Try to find or fetch Catch2
    find_package(Catch2 3 QUIET)
    if(NOT Catch2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.5.2
        )
        FetchContent_MakeAvailable(Catch2)
    endif()

    add_subdirectory(tests)
endif()

# Installation
install(TARGETS regent
    EXPORT regentTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT regentTargets
    FILE regentTargets.cmake
    NAMESPACE regent::
    DESTINATION lib/cmake/regent
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/regentConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/regentConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/regentConfig.cmake"
    INSTALL_DESTINATION lib/cmake/regent
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/regentConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/regentConfigVersion.cmake"
    DESTINATION lib/cmake/regent
)
