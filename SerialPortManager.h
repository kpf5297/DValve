#ifndef SERIAL_PORT_MANAGER_H
#define SERIAL_PORT_MANAGER_H

#include <string>
#include <vector>

class SerialPortManager {
private:
    static SerialPortManager* instance;
    int serialPortHandle;
    bool isConnected;

    // Private constructor for singleton pattern
    SerialPortManager();

    // Configures the serial port with given settings
    bool configureSerialPort(const std::string& portName, int baudRate);

public:
    // Delete copy constructor and assignment operator
    SerialPortManager(const SerialPortManager&) = delete;
    void operator=(const SerialPortManager&) = delete;

    // Gets the singleton instance of the manager
    static SerialPortManager* getInstance();

    // Lists available serial ports on the system
    std::vector<std::string> listAvailablePorts();

    // Allows the user to select a serial port interactively
    bool selectAndConnect(int baudRate = 38400);

    // Connects to the serial port with the specified settings
    bool connect(const std::string& portName, int baudRate = 38400);

    // Disconnects the serial port
    void disconnect();

    // Sends a command to the serial port and returns the response
    std::string sendCommand(const std::string& command);

    // Reads data from the serial port
    std::string readData();

    // Writes data to the serial port
    bool writeData(const std::string& data);

    // Destructor
    ~SerialPortManager();
};

#endif // SERIAL_PORT_MANAGER_H
