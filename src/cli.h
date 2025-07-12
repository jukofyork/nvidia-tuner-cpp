#pragma once

#include <string>
#include <optional>

struct Cli {
    unsigned int index = 0;
    std::optional<int> core_clock_offset;
    std::optional<int> memory_clock_offset;
    std::optional<unsigned int> power_limit;
    std::optional<std::string> pairs;
    unsigned int fan_speed_update_period = 2;
    unsigned int temperature_hysteresis = 5;
};

class CliParser {
public:
    static Cli parse(int argc, char* argv[]);
    static void print_help(const std::string& program_name);
    static void print_version();
    
private:
    static unsigned int validate_fan_speed_update_period(const std::string& value);
};