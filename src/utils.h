#pragma once

#include <vector>
#include <string>

struct TempFanPair {
    unsigned int temperature;
    unsigned int fan_speed;
};

namespace utils {
    std::vector<TempFanPair> parse_temperature_fan_speed_pairs(const std::string& pairs);
    void check_temperature_fan_speed_pairs(const std::vector<TempFanPair>& pairs);
    unsigned int interpolate_fan_speed(const std::vector<TempFanPair>& temp_fan_pairs, unsigned int current_temp);
    bool escalate_privileges();
}