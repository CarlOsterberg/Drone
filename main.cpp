#include <iostream>
#include <sensor_reader.h>

// Connect the white cable to pin 18
// black cable to ground
// red to 5V


int main()
{
    SensorReader sensor;
    int status = sensor.initialize();
    status |= sensor.start();
    if (status != 0)
    {
        std::cerr << "Failed to start sensor reader" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Type 'q' to quit:\n";
    while (true)
    {
        std::string s;
        std::cin >> s;
        if (s == "q")
        {
            return EXIT_SUCCESS;
        }
        std::cout << "Invalid input. Try again.\n";
    }
}
