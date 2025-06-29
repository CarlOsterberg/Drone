//
// Created by skipedibap on 2025-06-08.
//

#include "sensor_reader.h"

#include <iostream>
#include <pigpio.h>
#include <array>
#include <chrono>
#include <thread>

int SensorReader::m_i2c_handle;

SensorReader::~SensorReader()
{
    if (m_thread_handle != 0)
    {
        pthread_cancel(m_thread_handle);
        pthread_attr_destroy(&m_thread_attr);
    }
    if (m_i2c_handle != 0)
    {
        i2cClose(m_i2c_handle);
        gpioTerminate();
    }
}

int SensorReader::initialize()
{
    int attribute_status = pthread_attr_init(&m_thread_attr);
    if (attribute_status != 0)
    {
        std::cerr << "pthread_attr_init failed" << std::endl;
        return -1;
    }

    if (gpioInitialise() < 0)
    {
        std::cerr << "pigpio init failed" << std::endl;
        return -1;
    }

    m_i2c_handle = i2cOpen(m_i2c_bus, m_sensor_i2c_addr, 0);
    if (m_i2c_handle < 0)
    {
        std::cerr << "I2C open failed" << std::endl;
        return -1;
    }

    return 0;
}

int SensorReader::start()
{
    int i2c_status = i2cWriteByteData(m_i2c_handle, 0x6B, 0x00); // PWR_MGMT_1
    // i2c_status |= i2cWriteByteData(m_i2c_handle, 0x23, 0b0111'1000); // Configure FIFO
    // i2c_status |= i2cWriteByteData(m_i2c_handle, 0x6A, 0b0100'0101); // Enable FIFO
    if (i2c_status != 0)
    {
        std::cerr << "I2C startup failed" << std::endl;
        return -1;
    }

    int thread_status = pthread_create(&m_thread_handle, &m_thread_attr, &SensorReader::run, nullptr);
    if (thread_status != 0)
    {
        std::cerr << "pthread_create failed" << std::endl;
        return -1;
    }

    return 0;
}

void* SensorReader::run([[maybe_unused]] void* arg)
{
    SensorReading reading{};
    std::array<char, 14> buffer{};
    int status;
    for (;;)
    {
        // | Sensor  | Register Address        | Bytes | Description  |
        // | ------- | ----------------------- | ----- | ------------ |
        // | Accel X | 0x3B (high), 0x3C (low) | 2     | Accel X-axis |
        // | Accel Y | 0x3D, 0x3E              | 2     | Accel Y-axis |
        // | Accel Z | 0x3F, 0x40              | 2     | Accel Z-axis |
        // | Temp    | 0x41, 0x42              | 2     | Temperature  |
        // | Gyro X  | 0x43, 0x44              | 2     | Gyro X-axis  |
        // | Gyro Y  | 0x45, 0x46              | 2     | Gyro Y-axis  |
        // | Gyro Z  | 0x47, 0x48              | 2     | Gyro Z-axis  |
        // https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

        status = i2cReadI2CBlockData(m_i2c_handle, 0x3B, buffer.data(), buffer.size());

        // Note: Reading only FIFO_COUNT_L will not update the registers to the current sample count.
        // FIFO_COUNT_H must be accessed first to update the contents of both these registers.
        // FIFO_COUNT should always be read in high-low order in order to guarantee that the most current
        // FIFO Count value is read.
        // int FIFO_COUNT_H = i2cReadByteData(m_i2c_handle, 0x72); // FIFO_COUNT[15:8]
        // int FIFO_COUNT_L = i2cReadByteData(m_i2c_handle, 0x72); // FIFO_COUNT[7:0]
        if (status < 0)
        {
            std::cerr << "Failed to read sensor" << std::endl;
            continue;
        }
        reading.accelX = static_cast<int16_t>(buffer[0] << 8 | buffer[1]);
        reading.accelY = static_cast<int16_t>(buffer[2] << 8 | buffer[3]);
        reading.accelZ = static_cast<int16_t>(buffer[4] << 8 | buffer[5]);
        reading.gyroX = static_cast<int16_t>(buffer[8] << 8 | buffer[9]);
        reading.gyroY = static_cast<int16_t>(buffer[10] << 8 | buffer[11]);
        reading.gyroZ = static_cast<int16_t>(buffer[12] << 8 | buffer[13]);

        std::cout << "Accel [X,Y,Z]: " << reading.accelX << ", " << reading.accelY << ", " << reading.accelZ << std::endl;
        std::cout << "Gyro  [X,Y,Z]: " << reading.gyroX << ", " << reading.gyroY << ", " << reading.gyroZ << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
}
