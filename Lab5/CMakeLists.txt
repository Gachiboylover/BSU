cmake_minimum_required(VERSION 3.8)
project(Lab5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Server Server.cpp)
add_executable(Client Client.cpp)
