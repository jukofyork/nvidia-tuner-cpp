#pragma once

class TemperatureController {
private:
    const unsigned int target_temp;    // Target temperature (°C)
    const unsigned int min_fan_speed;  // Minimum fan speed (%)
    const unsigned int max_fan_speed;  // Maximum fan speed (%)
    const float kp;                    // Proportional gain
    const float ki;                    // Integral gain
    const float dt;                    // Sample time (seconds)

    float integral_error;

public:
    TemperatureController(unsigned int current_temp,
                          unsigned int current_fan_speed,
                          unsigned int target_temp,
                          unsigned int min_fan_speed,
                          unsigned int max_fan_speed,
                          float kp,
                          float ki,
                          float dt);

    unsigned int calculate_fan_speed(unsigned int current_temp);
};
