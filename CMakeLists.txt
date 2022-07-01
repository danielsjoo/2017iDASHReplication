cmake_minimum_required(VERSION 3.23.2)

project(heTraining)

add_executable(heTraining main.cpp)

find_package(SEAL 4.0 REQUIRED)
target_link_libraries(heTraining SEAL::seal)