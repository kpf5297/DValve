#include "TSI40xx.h"
#include <stdexcept>  // For std::runtime_error and std::invalid_argument
#include <sstream>    // For std::stringstream
#include <algorithm>  // For std::remove_if
#include <iomanip>    // For std::setw and std::setfill
#include <thread>     // For std::this_thread::sleep_for
#include <iostream>   // For std::cout and std::cerr
#include <cmath>      // For std::abs

TSI40xx::TSI40xx() {
    serialManager = SerialPortManager::getInstance();
    if (!serialManager->selectAndConnect(38400)) {
        throw std::runtime_error("Failed to establish a connection with the flow meter.");
    }
}

TSI40xx::~TSI40xx() {
    serialManager->disconnect();
}

std::string TSI40xx::getSerialNumber() {
    return serialManager->sendCommand("SN");
}

std::string TSI40xx::getModelNumber() {
    return serialManager->sendCommand("MN");
}

std::string TSI40xx::getFirmwareRevision() {
    return serialManager->sendCommand("REV");
}

std::string TSI40xx::getLastCalibrationDate() {
    return serialManager->sendCommand("DATE");
}

std::vector<std::string> TSI40xx::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        // Trim leading and trailing whitespace
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
        tokens.push_back(token);
    }
    return tokens;
}

void TSI40xx::interpretErrorCode(const std::string& response) {
    if (response.find("ERR") != std::string::npos) {
        int errorCode = std::stoi(response.substr(response.find("ERR") + 3));
        switch (errorCode) {
            case 1:
                throw std::runtime_error("Unrecognizable command – The flow meter uses the length of the command and the first few letters to recognize a valid command.");
            case 2:
                throw std::runtime_error("Number out of range – The number entered as the operand to a command was out of the specified range or unrecognizable.");
            case 3:
                throw std::runtime_error("Invalid mode – One or more requested options to a command were invalid.");
            case 4:
                throw std::runtime_error("Command not possible – The supplied operands describe a command that is beyond the functional capability of the flow meter.");
            case 8:
                throw std::runtime_error("Internal error – An internal failure was detected.");
            default:
                throw std::runtime_error("Unknown error code received.");
        }
    }
}

std::vector<Measurement> TSI40xx::requestData(char format, char flow, char temp, char pressure, int numSamples) {
    if (format != 'A' && format != 'B' && format != 'C') {
        throw std::invalid_argument("Invalid format. Use 'A' for ASCII, 'B' for binary, 'C' for ASCII with CR and LF.");
    }

    if ((flow != 'F' && flow != 'x') || (temp != 'T' && temp != 'x') || (pressure != 'P' && pressure != 'x')) {
        throw std::invalid_argument("Invalid data request characters. Use 'F' for flow, 'T' for temperature, 'P' for pressure, 'x' for no data.");
    }

    if (numSamples < 1 || numSamples > 1000) {
        throw std::out_of_range("Number of samples must be between 1 and 1000.");
    }

    std::stringstream command;
    command << "D" << format << flow << temp << pressure << std::setw(4) << std::setfill('0') << numSamples;
    std::string response = serialManager->sendCommand(command.str());

    interpretErrorCode(response);

    if (response.find("OK") != std::string::npos) {
        // Remove the "OK" part
        size_t pos = response.find("OK");
        response = response.substr(pos + 3);  // Skip "OK" and the following newline or space

        // Split the data by commas
        std::vector<std::string> values = split(response, ',');

        // Determine the expected number of values per sample
        int valuesPerSample = 0;
        if (flow != 'x') valuesPerSample++;
        if (temp != 'x') valuesPerSample++;
        if (pressure != 'x') valuesPerSample++;

        // Group values into sets and create Measurement structs
        std::vector<Measurement> measurements;
        for (size_t i = 0; i < values.size(); i += valuesPerSample) {
            if (i + valuesPerSample <= values.size()) {
                Measurement measurement;
                measurement.flow = flow != 'x' ? std::stod(values[i]) : 0.0;
                measurement.temperature = temp != 'x' ? std::stod(values[i + (flow != 'x' ? 1 : 0)]) : 0.0;
                measurement.pressure = pressure != 'x' ? std::stod(values[i + (flow != 'x' ? 1 : 0) + (temp != 'x' ? 1 : 0)]) : 0.0;
                measurements.push_back(measurement);
            }
        }
        return measurements;
    } else {
        throw std::runtime_error("Error in response: " + response);
    }
}

Measurement TSI40xx::pollSingleMeasurement() {
    auto data = requestData('A', 'F', 'T', 'P', 1);
    if (!data.empty()) {
        return data[0];  // Return the first (and only) set of measurements
    } else {
        throw std::runtime_error("Failed to retrieve single measurement.");
    }
}

bool TSI40xx::setSampleRate(int sampleRate) {
    if (sampleRate < 1 || sampleRate > 1000) {
        throw std::out_of_range("Sample rate must be between 1 and 1000 milliseconds.");
    }
    std::stringstream command;
    command << "SSR" << std::setw(4) << std::setfill('0') << sampleRate;
    std::string response = serialManager->sendCommand(command.str());

    interpretErrorCode(response);

    return response.find("OK") != std::string::npos;
}

bool TSI40xx::setBeginTrigger(char triggerSource, double triggerLevel) {
    if (triggerSource != 'F' && triggerSource != 'P') {
        throw std::invalid_argument("Invalid trigger source. Use 'F' for flow or 'P' for pressure.");
    }
    std::stringstream command;
    command << "SBT" << triggerSource << std::fixed << std::showpoint << std::setprecision(2) << (triggerLevel < 0 ? "-" : "+") << std::setw(6) << std::setfill('0') << std::abs(triggerLevel);
    std::string response = serialManager->sendCommand(command.str());

    interpretErrorCode(response);

    std::cout << "Set Begin Trigger Command: [" << command.str() << "]" << std::endl;
    std::cout << "Response: [" << response << "]" << std::endl;
    return response.find("OK") != std::string::npos;
}

bool TSI40xx::setEndTrigger(char triggerSource, double triggerLevel) {
    if (triggerSource != 'F' && triggerSource != 'P') {
        throw std::invalid_argument("Invalid trigger source. Use 'F' for flow or 'P' for pressure.");
    }
    std::stringstream command;
    command << "SET" << triggerSource << std::fixed << std::showpoint << std::setprecision(2) << (triggerLevel < 0 ? "-" : "+") << std::setw(6) << std::setfill('0') << std::abs(triggerLevel);
    std::string response = serialManager->sendCommand(command.str());

    interpretErrorCode(response);

    std::cout << "Set End Trigger Command: [" << command.str() << "]" << std::endl;
    std::cout << "Response: [" << response << "]" << std::endl;
    return response.find("OK") != std::string::npos;
}

std::string TSI40xx::readCurrentSettings(const std::string& setting) {
    std::string response = serialManager->sendCommand("R" + setting);
    interpretErrorCode(response);
    return response;
}

DeviceInfo TSI40xx::getDeviceInfo() {
    DeviceInfo info;
    info.serialNumber = getSerialNumber();
    info.modelNumber = getModelNumber();
    info.firmwareRevision = getFirmwareRevision();
    info.lastCalibrationDate = getLastCalibrationDate();
    return info;
}
