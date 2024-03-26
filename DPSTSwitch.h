#ifndef DPSTSWITCH_H
#define DPSTSWITCH_H

#include <gpiod.h>
#include <string>


class DPSTSwitch {
public:
    // Constructor and Destructor
    DPSTSwitch(const char* chipName, unsigned int noPin, unsigned int ncPin);
    virtual ~DPSTSwitch();

    // Public methods
    void toggleSwitch();     // Simulates toggling the switch between ON and OFF
    bool readNOPin() const;  // Reads the state of the N/O pin
    bool readNCPin() const;  // Reads the state of the N/C pin
    std::string getName() const;  // Returns the name of the switch

private:
    struct gpiod_chip* chip;       // Pointer to the GPIO chip
    struct gpiod_line* noLine;     // GPIO line for the N/O pin
    struct gpiod_line* ncLine;     // GPIO line for the N/C pin
    bool state;                    // Represents the state of the switch
    std::string name;                   // Name of the switch

    void initGPIO(const char* chipName, unsigned int noPin, unsigned int ncPin);
};

#endif // DPSTSWITCH_H
