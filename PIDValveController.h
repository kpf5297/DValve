#ifndef PIDVALVECONTROLLER_H
#define PIDVALVECONTROLLER_H

#include "TSI40xx.h"
#include "PiStepper.h"
#include <chrono>
#include <thread>

class PIDValveController {
private:
    TSI40xx* flowMeter;
    PiStepper* stepperMotor;
    double setPoint;
    double kp, ki, kd;
    double integral, previousError;
    std::mutex controlMutex;
    bool running;

    double calculatePID(double currentFlow);

public:
    PIDValveController(TSI40xx* flowMeter, PiStepper* stepperMotor, double setPoint, double kp, double ki, double kd);
    void setSetPoint(double setPoint);
    void setPIDConstants(double kp, double ki, double kd);
    void startControlLoop();
    void stopControlLoop();
    double getCurrentFlow();
    double getSetPoint();
    void getPIDConstants(double& kp, double& ki, double& kd);
};

#endif // PIDVALVECONTROLLER_H
