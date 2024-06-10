#include "PIDValveController.h"
#include <iostream>
#include <cmath>

PIDValveController::PIDValveController(TSI40xx* flowMeter, PiStepper* stepperMotor, double setPoint, double kp, double ki, double kd)
    : flowMeter(flowMeter), stepperMotor(stepperMotor), setPoint(setPoint), kp(kp), ki(ki), kd(kd), integral(0), previousError(0), running(false) {}

void PIDValveController::setSetPoint(double setPoint) {
    std::lock_guard<std::mutex> lock(controlMutex);
    this->setPoint = setPoint;
}

void PIDValveController::setPIDConstants(double kp, double ki, double kd) {
    std::lock_guard<std::mutex> lock(controlMutex);
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
}

double PIDValveController::calculatePID(double currentFlow) {
    double error = setPoint - currentFlow;
    integral += error;
    double derivative = error - previousError;
    previousError = error;
    return kp * error + ki * integral + kd * derivative;
}

void PIDValveController::startControlLoop() {
    running = true;
    std::thread([this]() {
        while (running) {
            Measurement measurement = flowMeter->pollSingleMeasurement();
            double currentFlow = measurement.flow;
            double controlSignal = calculatePID(currentFlow);

            // Apply a smoother control signal by scaling it down
            controlSignal = controlSignal / 10.0;

            // Limit the control signal to avoid excessive movements
            controlSignal = std::max(-10.0, std::min(10.0, controlSignal));

            // Convert control signal to steps and direction
            int direction = (controlSignal > 0) ? 1 : 0;
            int steps = static_cast<int>(std::abs(controlSignal)); // Convert control signal to steps

            if (steps > 0) {
                stepperMotor->moveStepsAsync(steps, direction, nullptr);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Increase control loop frequency
        }
    }).detach();
}

void PIDValveController::stopControlLoop() {
    running = false;
}

double PIDValveController::getCurrentFlow() {
    Measurement measurement = flowMeter->pollSingleMeasurement();
    return measurement.flow;
}

double PIDValveController::getSetPoint() {
    return setPoint;
}

void PIDValveController::getPIDConstants(double& kp, double& ki, double& kd) {
    kp = this->kp;
    ki = this->ki;
    kd = this->kd;
}
