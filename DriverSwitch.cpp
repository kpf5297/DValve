/*

To compile: g++ -std=c++11 -o DriverSwitch DriverSwitch.cpp DPSTSwitch.cpp -lgpiod -pthread


*/

#include <iostream>
#include <thread>
#include <atomic>

#include "DPSTSwitch.h"

void motorThread(DPSTSwitch& switchObj, std::atomic<bool>& running) {
    bool prevNoState = switchObj.readNOPin();
    bool prevNcState = switchObj.readNCPin();

    while (running) {
        bool noState = switchObj.readNOPin();
        bool ncState = switchObj.readNCPin();

        if (prevNoState && !noState && ncState) {
            std::cout << "Switch activated: " << switchObj.getName() << std::endl;
        }

        prevNoState = noState;
        prevNcState = ncState;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    DPSTSwitch bottomSwitch("gpiochip0", 21, 20);
    // DPSTSwitch topSwitch("gpiochip0", 12, 16);

    std::atomic<bool> running(true);

    std::thread bottomThread(motorThread, std::ref(bottomSwitch), std::ref(running));
    // std::thread topThread(motorThread, std::ref(topSwitch), std::ref(running));

    // Wait for user input to stop the threads
    std::cin.get();

    running = false;

    bottomThread.join();
    // topThread.join();

    return 0;
}
