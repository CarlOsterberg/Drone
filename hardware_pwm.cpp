#include <pigpio.h>
#include <iostream>
#include <unistd.h>


// Connect the white cable to pin 18
// black cable to ground
// red to 5V


int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed\n";
        return 1;
    }

    // Valid PWM pins 12, 13, 18, 19
    // see https://pinout.xyz/pinout/pwm
    int pwmPin = 18;

    // Set to output mode
    gpioSetMode(pwmPin, PI_OUTPUT);

    int num;
    // Set ESC to min throttle to make it stop beeping
    gpioHardwarePWM(pwmPin, 100, 100'000);

    std::cout << "Enter number 0-9 (type 'q' to quit):\n";
    while (true) {
        std::cin >> num;
        if (std::cin.fail()) {
            std::cin.clear(); // clear error
            std::string s;
            std::cin >> s;
            if (s == "q") break;
            std::cout << "Invalid input. Try again.\n";
        } else {
            int pwmDuty = 0;
            switch (num) {
                case 0:
                    pwmDuty = 100'000;
                    break;
                case 1:
                    pwmDuty = 112'000;
                    break;
                case 2:
                    pwmDuty = 124'000;
                    break;
                case 3:
                    pwmDuty = 136'000;
                    break;
                case 4:
                    pwmDuty = 148'000;
                    break;
                case 5:
                    pwmDuty = 160'000;
                    break;
                case 6:
                    pwmDuty = 172'000;
                    break;
                case 7:
                    pwmDuty = 184'000;
                    break;
                case 8:
                    pwmDuty = 190'000;
                break;
                case 9:
                    pwmDuty = 200'000;
                    break;
                default:
                    std::cerr << "illegal number try 0-9 or q to quit." << std::endl;
            }
            gpioHardwarePWM(pwmPin, 100, pwmDuty);
        }
    }

    gpioHardwarePWM(pwmPin, 0, 0); // Turn off PWM
    gpioTerminate();
    return 0;
}
