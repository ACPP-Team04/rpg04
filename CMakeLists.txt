cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(game)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(GTest REQUIRED)


file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS 
    "src/main/*.cpp" 
    "src/main/*.h"
)
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*main\\.cpp$") 

add_library(game-lib ${GAME_SOURCES})
set_property(TARGET game-lib PROPERTY CXX_STANDARD 20)
target_include_directories(game-lib PUBLIC src/main)  

add_executable(main src/main/main.cpp)
set_property(TARGET main PROPERTY CXX_STANDARD 20)
target_link_libraries(main PRIVATE game-lib)


file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS 
    "src/test/*.cpp" 
    "src/test/*.h"
)
add_executable(tests ${TEST_SOURCES})
set_property(TARGET tests PROPERTY CXX_STANDARD 23)
target_link_libraries(tests PRIVATE game-lib GTest::gtest_main) 

enable_testing()
add_test(NAME tests COMMAND tests)

add_custom_target(
    format
    COMMAND clang-format -i ${GAME_SOURCES} ${TEST_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting src..."
)
set_target_properties(tests PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}"
)