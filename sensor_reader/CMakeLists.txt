cmake_minimum_required(VERSION 4.0.1)
project(sensor_reader)

set(CMAKE_CXX_STANDARD 20)

add_library(sensor_reader_thread
        OBJECT
        sensor_reader.cpp
)

target_include_directories(sensor_reader_thread PUBLIC .)

target_link_libraries(sensor_reader_thread
        PUBLIC
        pigpio_interface
        pigpio
        pthread
        rt
)
