cmake_minimum_required(VERSION 3.10)
project(algobowl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix warning (See https://cmake.org/cmake/help/latest/policy/CMP0135.html)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# Include Boost (See https://www.boost.org/doc/libs/1_84_0/more/getting_started/unix-variants.html)
find_package(Boost 1.70.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Include GoogleTest (See https://google.github.io/googletest/quickstart-cmake.html#set-up-a-project)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Get source files
file(GLOB_RECURSE SOURCES src/*.hpp src/*.cpp)

set(MAIN_SOURCES ${SOURCES})
list(FILTER MAIN_SOURCES EXCLUDE REGEX ".*_test.cpp")

set(TEST_SOURCES ${SOURCES})
list(FILTER TEST_SOURCES EXCLUDE REGEX "src/(solver|verifier).cpp")

# Set compiler flags (See https://stackoverflow.com/a/3376483)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native")

if (MSVC)
  set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS "/W4")
else()
  set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS "-Wall -Wcast-align -Wcast-qual -Wconversion -Wextra -Wformat=2 -Wpedantic -Wpointer-arith -Wshadow -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wwrite-strings")
  set_source_files_properties(${SOURCES} PROPERTIES LINK_FLAGS "-Wl,--no-undefined -fsanitize=address -fsanitize=leak -fsanitize=undefined")
endif()

# Main executables
add_executable(solver src/solver.cpp)
add_executable(verifier src/verifier.cpp)

add_library(main ${MAIN_SOURCES})

target_link_libraries(solver main)
target_link_libraries(verifier main)
target_link_libraries(solver ${Boost_LIBRARIES})
target_link_libraries(verifier ${Boost_LIBRARIES})

# Testing executable
if (CMAKE_BUILD_TYPE MATCHES Debug)
  enable_testing()
  add_executable(
    tests
    ${TEST_SOURCES}
  )
  target_link_libraries(
    tests
    ${Boost_LIBRARIES}
  )
  target_link_libraries(
    tests
    GTest::gtest_main
  )
  include(GoogleTest)
  gtest_discover_tests(tests)
endif()
