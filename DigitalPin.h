/*
    Dependencies:
    sudo apt-get update
    sudo apt-get install gpiod libgpiod-dev

*/


#ifndef DIGITALPIN_H
#define DIGITALPIN_H

#include <gpiod.h>
#include <string>
#include <mutex>

class DigitalPin {
public:
    enum class Direction {
        Input,
        Output
    };

    DigitalPin(int pinNumber, Direction dir, const std::string& name = "");
    ~DigitalPin();

    bool read() const;
    void write(bool value);
    std::string getName() const;

private:
    int pinNumber_;
    Direction direction_;
    std::string name_;
    gpiod_chip* chip_;
    gpiod_line* line_;
    mutable std::mutex mutex_;  // Mutable to allow locking in const functions
};

#endif // DIGITALPIN_H
