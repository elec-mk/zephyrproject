cmake_minimum_required(VERSION 3.20.0)

set(BOARD nucleo_f401re)

find_package(Zephyr)
project(my_zephyr_app)

target_sources(app PRIVATE src/main.cpp)
target_sources(app PRIVATE src/HT16K33.cpp)
target_include_directories(app PRIVATE src/inc)