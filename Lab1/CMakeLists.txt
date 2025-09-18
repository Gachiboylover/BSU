cmake_minimum_required(VERSION 3.10)

project(LabWork LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Creator Creator.cpp)

add_executable(Reporter Reporter.cpp)

add_executable(Main Main.cpp)
target_link_libraries(Main PRIVATE ws2_32)

add_subdirectory(googletest)
enable_testing()
add_executable(tests tests.cpp)
target_link_libraries(tests gtest gtest_main)
add_test(NAME run_tests COMMAND tests)