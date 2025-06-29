#include <iostream>
#include <pigpio.h>
#include <unistd.h>

int16_t combineBytes(uint8_t high, uint8_t low) {
    return static_cast<int16_t>((high << 8) | low);
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed." << std::endl;
        return 1;
    }

    int bus = 1;
    int addr = 0x68;
    int handle = i2cOpen(bus, addr, 0);
    if (handle < 0) {
        std::cerr << "Failed to open I2C device." << std::endl;
        return 1;
    }

    // Wake up the MPU-6050
    i2cWriteByteData(handle, 0x6B, 0x00); // PWR_MGMT_1 = 0

    // Wait a bit
    usleep(100000); // 100 ms

    // | Sensor  | Register Address        | Bytes | Description  |
    // | ------- | ----------------------- | ----- | ------------ |
    // | Accel X | 0x3B (high), 0x3C (low) | 2     | Accel X-axis |
    // | Accel Y | 0x3D, 0x3E              | 2     | Accel Y-axis |
    // | Accel Z | 0x3F, 0x40              | 2     | Accel Z-axis |
    // | Temp    | 0x41, 0x42              | 2     | Temperature  |
    // | Gyro X  | 0x43, 0x44              | 2     | Gyro X-axis  |
    // | Gyro Y  | 0x45, 0x46              | 2     | Gyro Y-axis  |
    // | Gyro Z  | 0x47, 0x48              | 2     | Gyro Z-axis  |
    // see, https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

    // Read 14 bytes from 0x3B
    char buffer[14];
    int readResult = i2cReadI2CBlockData(handle, 0x3B, buffer, 14);
    if (readResult < 0) {
        std::cerr << "Failed to read sensor data." << std::endl;
    } else {
        int16_t accelX = combineBytes(buffer[0], buffer[1]);
        int16_t accelY = combineBytes(buffer[2], buffer[3]);
        int16_t accelZ = combineBytes(buffer[4], buffer[5]);
        int16_t tempRaw = combineBytes(buffer[6], buffer[7]);
        int16_t gyroX = combineBytes(buffer[8], buffer[9]);
        int16_t gyroY = combineBytes(buffer[10], buffer[11]);
        int16_t gyroZ = combineBytes(buffer[12], buffer[13]);

        double temperature = tempRaw / 340.0 + 36.53;

        std::cout << "Accel [X,Y,Z]: " << accelX << ", " << accelY << ", " << accelZ << std::endl;
        std::cout << "Gyro  [X,Y,Z]: " << gyroX << ", " << gyroY << ", " << gyroZ << std::endl;
        std::cout << "Temp (°C): " << temperature << std::endl;
    }

    i2cClose(handle);
    gpioTerminate();
    return 0;
}
