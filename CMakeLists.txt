cmake_minimum_required(VERSION 3.16)
project(Calculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Calculator
    main.cpp
    calculator.cpp
    calculator.h
)

target_link_libraries(Calculator PRIVATE 
    Qt6::Core 
    Qt6::Widgets
)