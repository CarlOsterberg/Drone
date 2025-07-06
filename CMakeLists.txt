cmake_minimum_required(VERSION 4.0.1)
project(untitled)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(Abstractions)
add_subdirectory(raspberry_pi_gpio_library)
add_subdirectory(sensor_reader)

add_library(pigpio_interface INTERFACE)
target_include_directories(pigpio_interface INTERFACE raspberry_pi_gpio_library)


add_executable(motorGo hardware_pwm.cpp)
target_link_libraries(motorGo
        PRIVATE
        pigpio_interface
        pigpio
        pthread
        rt
)

add_executable(readGyro i2c_gyroscope.cpp)
target_link_libraries(readGyro
        PRIVATE
        pigpio_interface
        pigpio
        pthread
        rt
)

add_executable(application main.cpp)
target_link_libraries(application
        PRIVATE
        sensor_reader_thread
)
