#pragma once

#include <string>
#include <optional>
#include "constants.h"

struct Cli {
    unsigned int index = 0;
    std::optional<int> core_clock_offset;
    std::optional<int> memory_clock_offset;
    std::optional<unsigned int> power_limit;
    std::optional<unsigned int> max_core_clock;
    std::optional<unsigned int> max_memory_clock;
    std::optional<unsigned int> target_temperature;
    unsigned int fan_speed_update_period = DEFAULT_FAN_SPEED_UPDATE_PERIOD;
    float proportional_gain = DEFAULT_PROPORTIONAL_GAIN;
    float integral_gain = DEFAULT_INTEGRAL_GAIN;
};

class CliParser {
public:
    static Cli parse(int argc, char* argv[]);
    static void print_help(const std::string& program_name);
    static void print_version();
    
private:
    static unsigned int validate_target_temperature(const std::string& value);
    static unsigned int validate_fan_speed_update_period(const std::string& value);
    static float validate_proportional_gain(const std::string& value);
    static float validate_integral_gain(const std::string& value);
};
