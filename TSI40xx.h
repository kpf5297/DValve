#ifndef TSI40XX_H
#define TSI40XX_H

#include "SerialPortManager.h"
#include <string>
#include <vector>

struct Measurement {
    double flow;
    double temperature;
    double pressure;
};

struct DeviceInfo {
    std::string serialNumber;
    std::string modelNumber;
    std::string firmwareRevision;
    std::string lastCalibrationDate;
};

class TSI40xx {
private:
    SerialPortManager* serialManager;

    std::vector<std::string> split(const std::string& str, char delimiter);
    void interpretErrorCode(const std::string& response);

public:
    TSI40xx();
    ~TSI40xx();

    std::string getSerialNumber();
    std::string getModelNumber();
    std::string getFirmwareRevision();
    std::string getLastCalibrationDate();

    DeviceInfo getDeviceInfo();

    std::vector<Measurement> requestData(char format, char flow, char temp, char pressure, int numSamples);
    Measurement pollSingleMeasurement();

    bool setSampleRate(int sampleRate);
    bool setBeginTrigger(char triggerSource, double triggerLevel);
    bool setEndTrigger(char triggerSource, double triggerLevel);
    std::string readCurrentSettings(const std::string& setting);
};

#endif // TSI40XX_H