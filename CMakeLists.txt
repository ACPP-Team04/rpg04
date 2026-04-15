cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(game)
if (MSVC)
    add_compile_options(/utf-8)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(GTest REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)
find_package(TGUI REQUIRED)
find_package(spdlog CONFIG REQUIRED)
add_compile_definitions(SPDLOG_SHARED_LIB)
add_compile_definitions(SPDLOG_COMPILED_LIB)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
find_package(GTest REQUIRED)
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
        "src/main/*.cpp"
        "src/main/*.hpp"
)
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

find_package(magic_enum CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
add_library(game-lib ${GAME_SOURCES})
set_property(TARGET game-lib PROPERTY CXX_STANDARD 20)
target_include_directories(game-lib PUBLIC src/main)  
target_link_libraries(game-lib PUBLIC SFML::Graphics SFML::Window SFML::System)
target_link_libraries(game-lib PRIVATE TGUI::TGUI)
target_link_libraries(game-lib PRIVATE magic_enum::magic_enum)
target_link_libraries(game-lib PRIVATE spdlog::spdlog)
target_include_directories(game-lib PRIVATE ${CMAKE_SOURCE_DIR}/external/tileson/include)
add_executable(main src/main/Implementation/main.cpp)
set_property(TARGET main PROPERTY CXX_STANDARD 20)
target_link_libraries(main PRIVATE game-lib)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(main PRIVATE magic_enum::magic_enum)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/external/tileson/include)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "src/test/*.cpp"
        "src/test/*.hpp"
)
add_executable(tests ${TEST_SOURCES})
set_property(TARGET tests PROPERTY CXX_STANDARD 23)
target_link_libraries(tests PRIVATE game-lib GTest::gtest_main)
target_link_libraries(tests PRIVATE magic_enum::magic_enum)
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(game-lib PRIVATE -O0 -g --coverage)
        target_link_options(game-lib PRIVATE --coverage)
        target_compile_options(tests PRIVATE -O0 -g --coverage)
        target_link_options(tests PRIVATE --coverage)
        target_compile_options(main PRIVATE -O0 -g --coverage)
        target_link_options(main PRIVATE --coverage)
    else()
        message(WARNING "Coverage is only configured for GCC and Clang.")
    endif()
endif()

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
