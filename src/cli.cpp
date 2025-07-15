#include "cli.h"
#include "constants.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>

Cli CliParser::parse(int argc, char* argv[]) {
    Cli cli;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            print_help(argv[0]);
            exit(0);
        } else if (arg == "-V" || arg == "--version") {
            print_version();
            exit(0);
        } else if (arg == "-g" || arg == "--gpu-index") {
            if (++i >= argc) throw std::runtime_error("Missing value for gpu-index");
            cli.index = std::stoul(argv[i]);
        } else if (arg == "-c" || arg == "--core-clock-offset") {
            if (++i >= argc) throw std::runtime_error("Missing value for core-clock-offset");
            cli.core_clock_offset = std::stoi(argv[i]);
        } else if (arg == "-m" || arg == "--memory-clock-offset") {
            if (++i >= argc) throw std::runtime_error("Missing value for memory-clock-offset");
            cli.memory_clock_offset = std::stoi(argv[i]);
        } else if (arg == "-C" || arg == "--max-core-clock") {
            if (++i >= argc) throw std::runtime_error("Missing value for max-core-clock");
            cli.max_core_clock = std::stoul(argv[i]);
        } else if (arg == "-M" || arg == "--max-memory-clock") {
            if (++i >= argc) throw std::runtime_error("Missing value for max-memory-clock");
            cli.max_memory_clock = std::stoul(argv[i]);
        } else if (arg == "-l" || arg == "--power-limit") {
            if (++i >= argc) throw std::runtime_error("Missing value for power-limit");
            cli.power_limit = std::stoul(argv[i]);
        } else if (arg == "-t" || arg == "--target-temperature") {
            if (++i >= argc) throw std::runtime_error("Missing value for target-temperature");
            cli.target_temperature = validate_target_temperature(argv[i]);
        } else if (arg == "-f" || arg == "--fan-speed-update-period") {
            if (++i >= argc) throw std::runtime_error("Missing value for fan-speed-update-period");
            cli.fan_speed_update_period = validate_fan_speed_update_period(argv[i]);
        } else if (arg == "-p" || arg == "--proportional-gain") {
            if (++i >= argc) throw std::runtime_error("Missing value for proportional-gain");
            cli.proportional_gain = validate_proportional_gain(argv[i]);
        } else if (arg == "-i" || arg == "--integral-gain") {
            if (++i >= argc) throw std::runtime_error("Missing value for integral-gain");
            cli.integral_gain = validate_integral_gain(argv[i]);
        } else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }
    
    return cli;
}

void CliParser::print_help(const std::string& program_name) {
    std::cout << "nvidia-tuner-cpp 0.1.0\n";
    std::cout << "A simple C++ CLI tool for overclocking, undervolting and controlling the fan of NVIDIA GPUs on Linux\n\n";
    std::cout << "USAGE:\n";
    std::cout << "    " << program_name << " [OPTIONS]\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "    -h, --help                           Print help information\n";
    std::cout << "    -V, --version                        Print version information\n";
    std::cout << "    -g, --gpu-index <INDEX>              GPU index [default: 0]\n";
    std::cout << "    -c, --core-clock-offset <OFFSET>     Core clock offset (MHz)\n";
    std::cout << "    -m, --memory-clock-offset <OFFSET>   Memory clock offset (MHz)\n";
    std::cout << "    -C, --max-core-clock <CLOCK>         Maximum boost core clock (MHz)\n";
    std::cout << "    -M, --max-memory-clock <CLOCK>       Maximum boost memory clock (MHz)\n";
    std::cout << "    -l, --power-limit <LIMIT>            Power limit (W)\n";
    std::cout << "    -t, --target-temperature <TEMP>      Target temperature for PI control (°C) ["
              << MIN_TARGET_TEMPERATURE << "-" << MAX_TARGET_TEMPERATURE << "]\n";
    std::cout << "    -f, --fan-speed-update-period <SEC>  Fan speed update period (s) [default: "
              << DEFAULT_FAN_SPEED_UPDATE_PERIOD << ", range: " << MIN_FAN_SPEED_UPDATE_PERIOD
              << "-" << MAX_FAN_SPEED_UPDATE_PERIOD << "]\n";
    std::cout << "    -p, --proportional-gain <GAIN>       PI proportional gain [default: "
              << DEFAULT_PROPORTIONAL_GAIN << "]\n";
    std::cout << "    -i, --integral-gain <GAIN>           PI integral gain [default: "
              << DEFAULT_INTEGRAL_GAIN << "]\n";
}

void CliParser::print_version() {
    std::cout << "nvidia-tuner-cpp 0.1.0\n";
}

unsigned int CliParser::validate_fan_speed_update_period(const std::string& value) {
    unsigned int period = std::stoul(value);
    if (period < MIN_FAN_SPEED_UPDATE_PERIOD || period > MAX_FAN_SPEED_UPDATE_PERIOD) {
        throw std::runtime_error("Fan speed update period must be between " +
                                std::to_string(MIN_FAN_SPEED_UPDATE_PERIOD) + " and " +
                                std::to_string(MAX_FAN_SPEED_UPDATE_PERIOD) + " seconds");
    }
    return period;
}

float CliParser::validate_proportional_gain(const std::string& value) {
    float gain = std::stof(value);
    if (gain < MIN_PROPORTIONAL_GAIN || gain > MAX_PROPORTIONAL_GAIN) {
        throw std::runtime_error("Proportional gain must be between " +
                                std::to_string(MIN_PROPORTIONAL_GAIN) + " and " +
                                std::to_string(MAX_PROPORTIONAL_GAIN));
    }
    return gain;
}

float CliParser::validate_integral_gain(const std::string& value) {
    float gain = std::stof(value);
    if (gain < MIN_INTEGRAL_GAIN || gain > MAX_INTEGRAL_GAIN) {
        throw std::runtime_error("Integral gain must be between " +
                                std::to_string(MIN_INTEGRAL_GAIN) + " and " +
                                std::to_string(MAX_INTEGRAL_GAIN));
    }
    return gain;
}

float CliParser::validate_target_temperature(const std::string& value) {
    float temp = std::stof(value);
    if (temp < MIN_TARGET_TEMPERATURE || temp > MAX_TARGET_TEMPERATURE) {
        throw std::runtime_error("Target temperature must be between " +
                                std::to_string(MIN_TARGET_TEMPERATURE) + "°C and " +
                                std::to_string(MAX_TARGET_TEMPERATURE) + "°C");
    }
    return temp;
}
