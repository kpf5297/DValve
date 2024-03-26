#include <gpiod.h>
#include <iostream>
#include <unistd.h>
#include <thread>

// Replace these with your actual GPIO chip and line numbers
const char* gpio_chip_name = "gpiochip0";
const unsigned int bottom_switch_line_num = 21; // Example GPIO line number for bottom switch
const unsigned int top_switch_line_num = 12; // Example GPIO line number for top switch

void monitorSwitch(const unsigned int line_num, const std::string& switch_name) {
    struct gpiod_chip* chip;
    struct gpiod_line* line;
    struct gpiod_line_event event;
    int ret;

    chip = gpiod_chip_open_by_name(gpio_chip_name);
    if (!chip) {
        std::cerr << "Failed to open " << gpio_chip_name << std::endl;
        return;
    }

    line = gpiod_chip_get_line(chip, line_num);
    if (!line) {
        std::cerr << "Failed to get line " << line_num << std::endl;
        gpiod_chip_close(chip);
        return;
    }

    ret = gpiod_line_request_falling_edge_events(line, switch_name.c_str());
    if (ret < 0) {
        std::cerr << "Failed to request events for " << switch_name << std::endl;
        gpiod_line_release(line);
        gpiod_chip_close(chip);
        return;
    }

    std::cout << "Monitoring " << switch_name << " for falling edge events." << std::endl;
    while (true) {
        ret = gpiod_line_event_wait(line, NULL);
        if (ret < 0) {
            std::cerr << "Error waiting for event on " << switch_name << std::endl;
            break;
        } else if (ret == 1) {
            // Event occurred
            ret = gpiod_line_event_read(line, &event);
            if (ret < 0) {
                std::cerr << "Error reading event from " << switch_name << std::endl;
                break;
            }
            // Here, handle the event (e.g., stop the motor)
            std::cout << switch_name << " triggered!" << std::endl;
            // For simplicity, we break after the event. Remove this to keep waiting.
            break;
        }
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
}

int main() {
    std::thread bottomSwitchThread(monitorSwitch, bottom_switch_line_num, "Bottom Switch");
    std::thread topSwitchThread(monitorSwitch, top_switch_line_num, "Top Switch");

    bottomSwitchThread.join();
    topSwitchThread.join();

    return 0;
}
