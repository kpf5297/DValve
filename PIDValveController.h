#ifndef PIDVALVECONTROLLER_H
#define PIDVALVECONTROLLER_H

#include "TSI40xx.h"
#include "PiStepper.h"
#include <atomic>
#include <mutex>
#include <thread>

class PIDValveController {
public:
    PIDValveController(TSI40xx* flowMeter, PiStepper* stepperMotor, double setPoint, double kp, double ki, double kd);

    void setSetPoint(double setPoint);
    void setPIDConstants(double kp, double ki, double kd);
    void startControlLoop();
    void stopControlLoop();
    double getCurrentFlow();
    double getSetPoint();
    void getPIDConstants(double& kp, double& ki, double& kd);

    // New method to adjust flow to target and stop once stable
    bool adjustFlowToTarget(double targetFlow, double tolerance, int stableCount);

private:
    TSI40xx* flowMeter;
    PiStepper* stepperMotor;
    double setPoint;
    double kp, ki, kd;
    double integral;
    double previousError;
    std::atomic<bool> running;
    std::mutex controlMutex;

    double calculatePID(double currentFlow);
};

#endif // PIDVALVECONTROLLER_H
