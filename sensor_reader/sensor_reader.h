//
// Created by skipedibap on 2025-06-08.
//

#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include <cstdint>
#include <pthread.h>

struct SensorReading
{
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
};

class SensorReader
{
public:
    int initialize();
    int start();
    ~SensorReader();

private:
    [[noreturn]] static void* run(void* arg);

    static constexpr int m_i2c_bus = 1;
    static constexpr int m_sensor_i2c_addr = 0x68;

    pthread_attr_t m_thread_attr{};
    pthread_t m_thread_handle{0};
    static int m_i2c_handle;
};

#endif //SENSOR_READER_H
