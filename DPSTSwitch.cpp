#include "DPSTSwitch.h"
#include <iostream>

// Constructor
DPSTSwitch::DPSTSwitch(const char* chipName, unsigned int noPin, unsigned int ncPin) : state(false) {
    initGPIO(chipName, noPin, ncPin);

    // Generate name based on the GPIO pins
    name = "Switch_" + std::to_string(noPin) + "_" + std::to_string(ncPin);
}

// Destructor
DPSTSwitch::~DPSTSwitch() {
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
    state = !state;
}

// Read the state of the Normally Open (N/O) pin
bool DPSTSwitch::readNOPin() const {
    return gpiod_line_get_value(noLine);
}

// Read the state of the Normally Closed (N/C) pin
bool DPSTSwitch::readNCPin() const {
    return gpiod_line_get_value(ncLine);
}

// Get the name of the switch
std::string DPSTSwitch::getName() const {

    return name;
}
