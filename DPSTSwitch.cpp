#include "DPSTSwitch.h"
#include <iostream>

// Constructor
DPSTSwitch::DPSTSwitch(const char* chipName, unsigned int noPin, unsigned int ncPin) : state(false) {
    // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
    initGPIO(chipName, noPin, ncPin);
}

// Destructor
DPSTSwitch::~DPSTSwitch() {
    // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
    if (noLine) {
        gpiod_line_release(noLine);
    }
    if (ncLine) {
        gpiod_line_release(ncLine);
    }
    if (chip) {
        gpiod_chip_close(chip);
    }
}

// Initialize GPIO pins
void DPSTSwitch::initGPIO(const char* chipName, unsigned int noPin, unsigned int ncPin) {
    // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
    chip = gpiod_chip_open_by_name(chipName);
    if (!chip) {
        std::cerr << "Failed to open GPIO chip" << std::endl;
        return;
    }

    noLine = gpiod_chip_get_line(chip, noPin);
    ncLine = gpiod_chip_get_line(chip, ncPin);

    if (!noLine || !ncLine) {
        std::cerr << "Failed to get GPIO line" << std::endl;
        if (chip) {
            gpiod_chip_close(chip);
        }
        return;
    }

    if (gpiod_line_request_input(noLine, "dpst_no") < 0 || gpiod_line_request_input(ncLine, "dpst_nc") < 0) {
        std::cerr << "Failed to request line as input" << std::endl;
        return;
    }
}

// Toggle the state of the switch
void DPSTSwitch::toggleSwitch() {
    std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
    state = !state;
}

// Read the state of the Normally Open (N/O) pin
bool DPSTSwitch::readNOPin() const {
    std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
    return gpiod_line_get_value(noLine);
}

// Read the state of the Normally Closed (N/C) pin
bool DPSTSwitch::readNCPin() const {
    std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
    return gpiod_line_get_value(ncLine);
}

// Get the name of the switch
std::string DPSTSwitch::getName() const {
    std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
    return gpiod_line_consumer(noLine);
}
