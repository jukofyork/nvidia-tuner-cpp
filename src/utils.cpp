#include "utils.h"
#include "constants.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

namespace utils {

TempFanPair parse_temperature_fan_speed_parts(const std::vector<std::string>& parts) {
    if (parts.size() != 2) {
        throw std::runtime_error("Invalid temperature and fan speed pair format");
    }
    
    unsigned int temperature = std::stoul(parts[0]);
    unsigned int fan_speed = std::stoul(parts[1]);
    
    if (temperature > MAX_TEMPERATURE) {
        throw std::runtime_error("Temperature within pairs exceeds limit of " + 
                               std::to_string(MAX_TEMPERATURE) + "°C: " + 
                               std::to_string(temperature) + "°C");
    }
    
    if (fan_speed > MAX_FAN_SPEED) {
        throw std::runtime_error("Fan speed within pairs exceeds limit of " + 
                               std::to_string(MAX_FAN_SPEED) + "%: " + 
                               std::to_string(fan_speed) + "%");
    }
    
    return {temperature, fan_speed};
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        tokens.push_back(token);
    }
    
    return tokens;
}

std::vector<TempFanPair> parse_temperature_fan_speed_pairs(const std::string& pairs) {
    std::vector<TempFanPair> result;
    auto pair_strings = split(pairs, ',');
    
    for (const auto& pair : pair_strings) {
        auto parts = split(pair, ':');
        result.push_back(parse_temperature_fan_speed_parts(parts));
    }
    
    return result;
}

void check_temperature_fan_speed_pairs(const std::vector<TempFanPair>& pairs) {
    if (pairs.empty()) {
        throw std::runtime_error("No temperature fan speed pair provided");
    }
    
    for (size_t i = 1; i < pairs.size(); ++i) {
        if (pairs[i].temperature <= pairs[i-1].temperature) {
            throw std::runtime_error("Temperature is not increasing");
        }
        if (pairs[i].fan_speed < pairs[i-1].fan_speed) {
            throw std::runtime_error("Fan speed is decreasing");
        }
    }
}

unsigned int interpolate_fan_speed(const std::vector<TempFanPair>& temp_fan_pairs, 
                                 unsigned int current_temp) {
    size_t num_pairs = temp_fan_pairs.size();
    
    // Handle out-of-bounds temperatures and single pair
    if (current_temp <= temp_fan_pairs[0].temperature) {
        return temp_fan_pairs[0].fan_speed;
    }
    if (current_temp >= temp_fan_pairs[num_pairs - 1].temperature) {
        return temp_fan_pairs[num_pairs - 1].fan_speed;
    }
    
    // Find the appropriate range for interpolation
    for (size_t i = 1; i < num_pairs; ++i) {
        const auto& lower = temp_fan_pairs[i - 1];
        const auto& upper = temp_fan_pairs[i];
        
        if (current_temp >= lower.temperature && current_temp <= upper.temperature) {
            // Linear interpolation
            float result = (static_cast<float>(lower.fan_speed) * 
                          (static_cast<float>(upper.temperature) - static_cast<float>(current_temp)) +
                          static_cast<float>(upper.fan_speed) * 
                          (static_cast<float>(current_temp) - static_cast<float>(lower.temperature))) /
                          (static_cast<float>(upper.temperature) - static_cast<float>(lower.temperature));
            
            return static_cast<unsigned int>(result + 0.5f); // Round to nearest
        }
    }
    
    // Should never reach here
    return MAX_FAN_SPEED;
}

bool command_exists(const std::string& command) {
    std::string check_cmd = "which " + command + " >/dev/null 2>&1";
    return system(check_cmd.c_str()) == 0;
}

bool escalate_privileges() {
    if (getuid() == 0) {
        return true; // Already running as root
    }
    
    std::string program_path;
    {
        char buffer[1024];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1) {
            buffer[len] = '\0';
            program_path = buffer;
        } else {
            return false;
        }
    }
    
    // Try different privilege escalation methods
    if (command_exists("sudo")) {
        execl("/usr/bin/sudo", "sudo", program_path.c_str(), nullptr);
    } else if (command_exists("doas")) {
        execl("/usr/bin/doas", "doas", program_path.c_str(), nullptr);
    } else if (command_exists("pkexec")) {
        execl("/usr/bin/pkexec", "pkexec", program_path.c_str(), nullptr);
    }
    
    return false;
}

} // namespace utils