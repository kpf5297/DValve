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

    // Implement integral windup protection
    double maxIntegral = 50.0;
    if (integral > maxIntegral) integral = maxIntegral;
    if (integral < -maxIntegral) integral = -maxIntegral;

    return kp * error + ki * integral + kd * derivative;
}

void PIDValveController::startControlLoop() {
    running = true;
    std::thread([this]() {
        while (running) {
            Measurement measurement = flowMeter->pollSingleMeasurement();
            double currentFlow = measurement.flow;
            double controlSignal = calculatePID(currentFlow);

            // Print the target and read values with detailed debug information
            std::cout << "Target: " << setPoint << ", Read: " << currentFlow << ", Control Signal: " << controlSignal 
                      << ", kp*error: " << kp*(setPoint - currentFlow) << ", ki*integral: " << ki*integral 
                      << ", kd*derivative: " << kd*(currentFlow - previousError) << std::endl;

            // Implement a deadband to avoid small adjustments around the target
            const double deadband = 1.0;
            if (std::abs(setPoint - currentFlow) < deadband) {
                controlSignal = 0;
            }

            // Implement a minimum threshold for control signal
            const double minControlSignal = 10.0;
            if (std::abs(controlSignal) < minControlSignal) {
                controlSignal = 0;
            }

            // Apply a smoother control signal by scaling it down
            controlSignal = controlSignal / 5.0;

            // Limit the control signal to avoid excessive movements
            controlSignal = std::max(-50.0, std::min(50.0, controlSignal));

            // Convert control signal to steps and direction
            int direction = (controlSignal > 0) ? 1 : 0;
            int steps = static_cast<int>(std::abs(controlSignal)); // Convert control signal to steps

            if (steps > 0) {
                stepperMotor->moveStepsAsync(steps, direction, nullptr);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust control loop frequency as needed
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

// New method to adjust flow to target and stop once stable
bool PIDValveController::adjustFlowToTarget(double targetFlow, double tolerance, int stableCount) {
    setSetPoint(targetFlow);
    int count = 0;

    while (count < stableCount) {
        double currentFlow = getCurrentFlow();
        double error = targetFlow - currentFlow;

        // Print current flow and error for debugging
        std::cout << "Adjusting Flow: Target: " << targetFlow << ", Current: " << currentFlow << ", Error: " << error << std::endl;

        double controlSignal = calculatePID(currentFlow);

        // Print control signal details for debugging
        std::cout << "Control Signal: " << controlSignal 
                  << ", kp*error: " << kp * error 
                  << ", ki*integral: " << ki * integral 
                  << ", kd*derivative: " << kd * (error - previousError) << std::endl;

        // Convert control signal to steps and direction
        int direction = (controlSignal > 0) ? 1 : 0;
        int steps = static_cast<int>(std::abs(controlSignal)); // Convert control signal to steps

        if (steps > 0) {
            stepperMotor->moveStepsAsync(steps, direction, nullptr);
        }

        // Check if the error is within the tolerance range
        if (std::abs(error) < tolerance) {
            count++;
        } else {
            count = 0; // Reset count if flow is not within tolerance
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait before next check
    }

    stopControlLoop(); // Stop the control loop once stable
    return true; // Return true to indicate the flow is now stable at the target
}
