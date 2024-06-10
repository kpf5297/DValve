/*
 * Driver.cpp
 *
 * Compile with:
 * g++ -o tsiApp SerialPortManager.cpp TSI40xx.cpp Driver.cpp -std=c++17 -lstdc++fs
 */

#include "TSI40xx.h"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    try {
        TSI40xx tsiDevice;

        DeviceInfo deviceInfo = tsiDevice.getDeviceInfo();
        std::cout << "Serial Number: " << deviceInfo.serialNumber << std::endl;
        std::cout << "Model Number: " << deviceInfo.modelNumber << std::endl;
        std::cout << "Firmware Revision: " << deviceInfo.firmwareRevision << std::endl;
        std::cout << "Last Calibration Date: " << deviceInfo.lastCalibrationDate << std::endl;

        // Set sample rate
        if (tsiDevice.setSampleRate(5)) {
            std::cout << "Sample rate set successfully." << std::endl;
        }

        // Delay to ensure the device processes the command
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Set begin and end triggers
        // if (tsiDevice.setBeginTrigger('F', 0.00)) {
        //     std::cout << "Begin trigger set successfully." << std::endl;
        // }

        // Delay to ensure the device processes the command
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // if (tsiDevice.setEndTrigger('F', 200.00)) {
        //     std::cout << "End trigger set successfully." << std::endl;
        // }

        // Delay to ensure the device processes the command
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Check current settings
        std::cout << "Current Sample Rate: " << tsiDevice.readCurrentSettings("SR") << std::endl;

        // Delay before polling measurements
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Poll a single set of measurements
        try {
            auto singleMeasurement = tsiDevice.pollSingleMeasurement();

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            std::cout << "Flow: " << singleMeasurement.flow
                      << ", Temperature: " << singleMeasurement.temperature
                      << ", Pressure: " << singleMeasurement.pressure << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "An error occurred: " << e.what() << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }

    return 0;
}
