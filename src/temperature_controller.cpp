#include "temperature_controller.h"
#include "constants.h"
#include <algorithm>
#include <cmath>

TemperatureController::TemperatureController(float target, float p_gain, float i_gain, float sample_time, float baseline)
    : target_temp(target), kp(p_gain), ki(i_gain), dt(sample_time), baseline_fan_speed(baseline) {}

unsigned int TemperatureController::calculate_fan_speed(float current_temp) {
    float error = current_temp - target_temp;

    // Proportional term
    float p_term = kp * error;

    // Integral term
    integral_error += error * dt;
    float i_term = ki * integral_error;

    // Combine terms
    float output = baseline_fan_speed + p_term + i_term;

    // Anti-windup: clamp through integral term
    const float upper = static_cast<float>(MAX_FAN_SPEED);
    const float lower = baseline_fan_speed;

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

void TemperatureController::reset() {
    integral_error = 0.0f;
}

void TemperatureController::set_target_temperature(float target) {
    target_temp = target;
    reset(); // Reset integral when target changes
}

float TemperatureController::get_target_temperature() const {
    return target_temp;
}
