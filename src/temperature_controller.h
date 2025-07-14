#pragma once

#include "constants.h"

class TemperatureController {
private:
    float target_temp;
    float kp;  // Proportional gain
    float ki;  // Integral gain
    float integral_error = 0.0f;
    float dt;  // Sample time in seconds
    float baseline_fan_speed;  // Minimum fan speed

public:
    TemperatureController(float target, float p_gain, float i_gain, float sample_time, float baseline = BASELINE_FAN_SPEED);

    unsigned int calculate_fan_speed(float current_temp);
    void reset();
    void set_target_temperature(float target);
    float get_target_temperature() const;
};
