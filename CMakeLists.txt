cmake_minimum_required(VERSION 3.22)

project(STM32-INA226
        VERSION 1.1.0
        LANGUAGES CXX
)

add_library(STM32-INA226 INTERFACE)

target_include_directories(STM32-INA226
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)