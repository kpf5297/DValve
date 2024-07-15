# Valve Control System

This project controls a stepper motor to operate a valve system using a Raspberry Pi. The system includes a graphical user interface (GUI) to control and monitor the valve's position.

## Features

- Calibrate the motor to determine the full range of motion.
- Move the valve to fully open or fully closed positions.
- Move the valve to a user-defined position or by a user-defined number of steps.
- Display the valve position as a percentage.
- Emergency stop functionality.
- Log messages for user actions and system events.

## Prerequisites

Before you can build and run this project, you need to install the following dependencies on your Raspberry Pi.

### Dependencies

1. **gpiod library**: This library is used for GPIO control.
    ```bash
    sudo apt-get update
    sudo apt-get install gpiod libgpiod-dev
    ```

2. **Qt5 Libraries**: These libraries are used for the GUI.
    ```bash
    sudo apt-get install qt5-default
    ```

3. **C++ Build Tools**: Required for compiling the C++ code.
    ```bash
    sudo apt-get install build-essential
    ```

### Additional Setup Steps

1. **Clone the repository**:
    ```bash
    git clone https://github.com/kpf5297/DValve.git
    cd DValve
    git checkout version_2_initial
    ```

2. **Ensure the following files are in place**:
    - `mainwindow.ui`: The Qt Designer UI file.
    - `PiStepper.cpp` and `PiStepper.h`: The stepper motor control class.
    - `mainwindow.cpp` and `mainwindow.h`: The main window and logic for the GUI.
    - `resources.qrc`: The Qt resource file containing images.

3. **Create a desktop shortcut**:
    - Follow the instructions provided earlier to create a desktop shortcut for easy access to the application.

## Building the Project

1. **Compile the Project**:
    ```bash
    g++ -o PiStepperDriver PiStepperDriver.cpp PiStepper.cpp mainwindow.cpp -lgpiod -pthread -lQt5Widgets -lQt5Core -lQt5Gui
    ```

2. **Running the Application**:
    ```bash
    ./PiStepperDriver
    ```

## Usage

1. **Launch the Application**: Double-click the desktop shortcut or run the compiled binary as shown above.

2. **Calibrate the Motor**: On the start page, ensure all connections are correct and click "OK" to start the calibration process.

3. **Control the Valve**:
    - **Absolute Control**: Move the valve to a specific percentage open position.
    - **Relative Control**: Move the valve a specific number of steps either open or closed.
    - **Quick Moves**: Pre-defined quick move buttons for frequently used positions.

4. **Monitor the Valve**: The progress bar at the top of the GUI shows the current position of the valve.

5. **Emergency Stop**: Click the "Emergency Stop" button to immediately stop all motor operations.

## Troubleshooting

- **Dependencies**: Ensure all required libraries are installed. Missing dependencies will prevent the application from compiling or running correctly.
- **Permissions**: Make sure you have the necessary permissions to access GPIO pins and execute the compiled binary.
- **Hardware Connections**: Double-check all physical connections to the Raspberry Pi and ensure they match the expected configuration.

## Acknowledgements

- The Qt Project for providing an excellent framework for building GUIs.
- The Raspberry Pi Foundation for creating such a versatile platform.

