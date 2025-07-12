#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstdlib>

#include "cli.h"
#include "gpu_device.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    try {
        Cli cli = CliParser::parse(argc, argv);
        
        if (!utils::escalate_privileges()) {
            std::cerr << "Root privileges are required to run this command." << std::endl;
            return 1;
        }
        
        // Initialize NVML
        check_nvml_error(nvmlInit(), "initialize NVML");
        check_driver_version();
        
        nvmlDevice_t device_handle;
        check_nvml_error(nvmlDeviceGetHandleByIndex(cli.index, &device_handle),
                        "get GPU device");
        
        auto device = std::make_shared<NvmlDevice>(device_handle);
        
        // Set overclocking parameters
        if (cli.core_clock_offset.has_value()) {
            device->set_core_clock_offset(cli.core_clock_offset.value());
        }
        
        if (cli.memory_clock_offset.has_value()) {
            device->set_memory_clock_offset(cli.memory_clock_offset.value());
        }
        
        if (cli.power_limit.has_value()) {
            device->set_power_limit(cli.power_limit.value());
        }
        
        // Fan control
        if (cli.pairs.has_value()) {
            auto temp_fan_pairs = utils::parse_temperature_fan_speed_pairs(cli.pairs.value());
            utils::check_temperature_fan_speed_pairs(temp_fan_pairs);
            
            device->setup_cleanup();
            
            unsigned int hyst_upper_temp = 0;
            int last_fan_speed = -1;
            unsigned int first_temp = temp_fan_pairs[0].temperature;
            
            while (true) {
                unsigned int temperature = device->get_temperature();
                
                // Apply hysteresis to temperature
                if (cli.temperature_hysteresis > 0) {
                    if ((temperature < hyst_upper_temp) && 
                        ((temperature + cli.temperature_hysteresis) >= hyst_upper_temp)) {
                        if (temperature > first_temp) {
                            temperature = hyst_upper_temp;
                        }
                    } else {
                        hyst_upper_temp = temperature;
                    }
                }
                
                unsigned int fan_speed = utils::interpolate_fan_speed(temp_fan_pairs, temperature);
                
                // Set fan speed if it has changed
                if (last_fan_speed != static_cast<int>(fan_speed)) {
                    device->set_fan_speed(fan_speed);
                    last_fan_speed = static_cast<int>(fan_speed);
                }
                
                std::this_thread::sleep_for(std::chrono::seconds(cli.fan_speed_update_period));
            }
        }
        
        nvmlShutdown();
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        nvmlShutdown();
        return 1;
    }
}
