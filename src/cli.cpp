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
        } else if (arg == "-i" || arg == "--index") {
            if (++i >= argc) throw std::runtime_error("Missing value for index");
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
        } else if (arg == "-p" || arg == "--pairs") {
            if (++i >= argc) throw std::runtime_error("Missing value for pairs");
            cli.pairs = argv[i];
        } else if (arg == "-f" || arg == "--fan-speed-update-period") {
            if (++i >= argc) throw std::runtime_error("Missing value for fan-speed-update-period");
            cli.fan_speed_update_period = validate_fan_speed_update_period(argv[i]);
        } else if (arg == "-t" || arg == "--temperature-hysteresis") {
            if (++i >= argc) throw std::runtime_error("Missing value for temperature-hysteresis");
            cli.temperature_hysteresis = std::stoul(argv[i]);
        } else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }
    
    return cli;
}

void CliParser::print_help(const std::string& program_name) {
    std::cout << "nvidia-tuner 0.3.0\n";
    std::cout << "A simple CLI tool for overlocking, undervolting and controlling the fan of NVIDIA GPUs on Linux\n\n";
    std::cout << "USAGE:\n";
    std::cout << "    " << program_name << " [OPTIONS]\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "    -h, --help                           Print help information\n";
    std::cout << "    -V, --version                        Print version information\n";
    std::cout << "    -i, --index <INDEX>                  GPU index [default: 0]\n";
    std::cout << "    -c, --core-clock-offset <OFFSET>     Core clock offset (MHz)\n";
    std::cout << "    -m, --memory-clock-offset <OFFSET>   Memory clock offset (MHz)\n";
    std::cout << "    -C, --max-core-clock <CLOCK>         Maximum boost core clock (MHz)\n";
    std::cout << "    -M, --max-memory-clock <CLOCK>       Maximum boost memory clock (MHz)\n";
    std::cout << "    -l, --power-limit <LIMIT>            Power limit (W)\n";
    std::cout << "    -p, --pairs <PAIRS>                  Temperature (°C) and fan speed (%) pairs\n";
    std::cout << "                                         Format: temp1:fan1,temp2:fan2,...\n";
    std::cout << "    -f, --fan-speed-update-period <SEC>  Fan speed update period (s) [default: 2]\n";
    std::cout << "    -t, --temperature-hysteresis <DEG>   Temperature hysteresis (°C) [default: 5]\n";
}

void CliParser::print_version() {
    std::cout << "nvidia-tuner 0.3.0\n";
}

unsigned int CliParser::validate_fan_speed_update_period(const std::string& value) {
    unsigned int period = std::stoul(value);
    if (period > MAX_FAN_SPEED_UPDATE_PERIOD) {
        throw std::runtime_error("Fan speed update period exceeds limit of " + 
                                std::to_string(MAX_FAN_SPEED_UPDATE_PERIOD));
    }
    return period;
}
