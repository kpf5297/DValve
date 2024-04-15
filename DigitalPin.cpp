#include "DigitalPin.h"
#include <stdexcept>
#include <cstring> // For std::strerror

DigitalPin::DigitalPin(int pinNumber, Direction dir, const std::string& name)
    : pinNumber_(pinNumber), direction_(dir), name_(name.empty() ? "Pin" + std::to_string(pinNumber) : name) {
    chip_ = gpiod_chip_open_by_number(0);  // Assumes chip0
    if (!chip_) {
        throw std::runtime_error("Failed to open GPIO chip");
    }

    line_ = gpiod_chip_get_line(chip_, pinNumber_);
    if (!line_) {
        gpiod_chip_close(chip_);
        throw std::runtime_error("Failed to get line");
    }

    gpiod_line_request_config config;
    config.request_type = (dir == Direction::Input) ? GPIOD_LINE_REQUEST_DIRECTION_INPUT
                                                    : GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;
    config.consumer = "DigitalPin";
    config.flags = 0; // No additional flags

    if (gpiod_line_request(line_, &config, 0) != 0) {
        gpiod_chip_close(chip_);
        throw std::runtime_error("Failed to request line: " + std::string(std::strerror(errno)));
    }
}

DigitalPin::~DigitalPin() {
    if (line_) {
        gpiod_line_release(line_);
    }
    if (chip_) {
        gpiod_chip_close(chip_);
    }
}


bool DigitalPin::read() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (direction_ != Direction::Input) {
        throw std::runtime_error("Attempt to read from an output pin");
    }
    return gpiod_line_get_value(line_) > 0;
}

void DigitalPin::write(bool value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (direction_ != Direction::Output) {
        throw std::runtime_error("Attempt to write to an input pin");
    }
    gpiod_line_set_value(line_, value ? 1 : 0);
}

std::string DigitalPin::getName() const {
    return name_;
}
