#include "SerialPortManager.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <cstring>      // String function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <termios.h>    // POSIX terminal control definitions
#include <filesystem>   // For listing directory contents

SerialPortManager* SerialPortManager::instance = nullptr;

SerialPortManager::SerialPortManager() : serialPortHandle(-1), isConnected(false) {}

bool SerialPortManager::configureSerialPort(const std::string& portName, int baudRate) {
    serialPortHandle = open(portName.c_str(), O_RDWR | O_NOCTTY);
    if (serialPortHandle == -1) {
        std::cerr << "Error opening port " << strerror(errno) << std::endl;
        return false;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serialPortHandle, &tty) != 0) {
        std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
        close(serialPortHandle);
        return false;
    }

    cfsetospeed(&tty, B38400);
    cfsetispeed(&tty, B38400);

    tty.c_cflag &= ~PARENB; // No Parity
    tty.c_cflag &= ~CSTOPB; // 1 Stop bit
    tty.c_cflag |= CS8; // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // No hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty.c_oflag &= ~OPOST; // No Output Processing
    tty.c_oflag &= ~ONLCR; // Don't convert linefeeds

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Make raw

    tty.c_cc[VTIME] = 10; // Read timeout
    tty.c_cc[VMIN] = 0; // 1 byte at a time

    if (tcsetattr(serialPortHandle, TCSANOW, &tty) != 0) {
        std::cerr << "Error from tcsetattr: " << strerror(errno) << std::endl;
        close(serialPortHandle);
        return false;
    }

    isConnected = true;
    return true;
}

SerialPortManager* SerialPortManager::getInstance() {
    if (instance == nullptr) {
        instance = new SerialPortManager();
    }
    return instance;
}

std::vector<std::string> SerialPortManager::listAvailablePorts() {
    std::vector<std::string> ports;
    std::string basePath = "/dev/";

    for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
        std::string path = entry.path();
        if (path.find("ttyUSB") != std::string::npos || path.find("ttyACM") != std::string::npos || path.find("ttyS") != std::string::npos) {
            ports.push_back(path);
        }
    }
    return ports;
}

bool SerialPortManager::selectAndConnect(int baudRate) {
    auto ports = listAvailablePorts();
    if (ports.empty()) {
        std::cerr << "No serial ports found." << std::endl;
        return false;
    }

    std::cout << "Available Serial Ports:" << std::endl;
    for (size_t i = 0; i < ports.size(); ++i) {
        std::cout << i + 1 << ": " << ports[i] << std::endl;
    }

    std::cout << "Select a port to connect: ";
    int choice;
    std::cin >> choice;

    if (choice < 1 || choice > static_cast<int>(ports.size())) {
        std::cerr << "Invalid choice." << std::endl;
        return false;
    }

    return connect(ports[choice - 1], baudRate);
}

bool SerialPortManager::connect(const std::string& portName, int baudRate) {
    return configureSerialPort(portName, baudRate);
}

void SerialPortManager::disconnect() {
    if (isConnected && serialPortHandle != -1) {
        close(serialPortHandle);
        isConnected = false;
    }
}

std::string SerialPortManager::sendCommand(const std::string& command) {
    std::string fullCommand = command + "\r";  // Use carriage return only
    std::cout << "Sending command: [" << fullCommand << "]" << std::endl;
    if (!writeData(fullCommand)) {
        std::cerr << "Failed to write data." << std::endl;
        return "";
    }
    usleep(500000); // Wait for response
    std::string response = readData();
    std::cout << "Raw response: [" << response << "]" << std::endl;
    return response;
}

std::string SerialPortManager::readData() {
    char buffer[1024];
    std::string result;
    int n = read(serialPortHandle, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = 0; // Null-terminate the string
        result = buffer;
    } else if (n < 0) {
        std::cerr << "Error reading: " << strerror(errno) << std::endl;
    }
    return result; // Return the raw string, handle parsing or trimming CR/LF in calling function if needed
}

bool SerialPortManager::writeData(const std::string& data) {
    std::string command = data + "\r"; // Add carriage return to command
    int numBytes = write(serialPortHandle, command.c_str(), command.length());
    if (numBytes < 0) {
        std::cerr << "Error writing: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

SerialPortManager::~SerialPortManager() {
    disconnect();
}