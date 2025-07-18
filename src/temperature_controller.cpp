#include "temperature_controller.h"
#include <cmath>

TemperatureController::TemperatureController(unsigned int current_temp,
                                             unsigned int current_fan_speed,
                                             unsigned int target_temp,
                                             unsigned int min_fan_speed,
                                             unsigned int max_fan_speed,
                                             float kp,
                                             float ki,
                                             float dt)
    : target_temp(target_temp), min_fan_speed(min_fan_speed), max_fan_speed(max_fan_speed),
      kp(kp), ki(ki), dt(dt) {

    integral_error = 0.0f;
    if (ki > 0.0f) {
        float error = static_cast<float>(current_temp) - static_cast<float>(target_temp);

        // Proportional term
        float p_term = kp * error;

        // Solve for integral_error: current_fan_speed = min_fan_speed + p_term + ki * integral_error
        integral_error = (static_cast<float>(current_fan_speed) - static_cast<float>(min_fan_speed) - p_term) / ki;

        // Roll one step back (so we can keep the order calculate_fan_speed() and not add 1 time-step of lag)
        integral_error -= error * dt;
    }
}

unsigned int TemperatureController::calculate_fan_speed(unsigned int current_temp) {
    float error = static_cast<float>(current_temp) - static_cast<float>(target_temp);

    // Proportional term
    float p_term = kp * error;

    // Integral term
    integral_error += error * dt;
    float i_term = ki * integral_error;

    // Combine terms
    float output = static_cast<float>(min_fan_speed) + p_term + i_term;

    // Anti-windup: clamp through integral term
    const float lower = static_cast<float>(min_fan_speed);
    const float upper = static_cast<float>(max_fan_speed);

    if (output > upper && ki > 0.0f) {
        float excess = output - upper;
        integral_error -= excess / ki;
        output = upper;
    } else if (output < lower && ki > 0.0f) {
        float deficit = lower - output;
        integral_error += deficit / ki;
        output = lower;
    }

    return static_cast<unsigned int>(std::round(output));
}
