#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstdlib>

#include "cli.h"
#include "gpu_device.h"
#include "temperature_controller.h"
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

        if (cli.max_core_clock.has_value()) {
            device->set_max_core_clock(cli.max_core_clock.value());
        }

        if (cli.max_memory_clock.has_value()) {
            device->set_max_memory_clock(cli.max_memory_clock.value());
        }

        if (cli.power_limit.has_value()) {
            device->set_power_limit(cli.power_limit.value());
        }
        
        // PI temperature control
        if (cli.target_temperature.has_value()) {
            TemperatureController controller(
                cli.target_temperature.value(),
                cli.proportional_gain,
                cli.integral_gain,
                static_cast<float>(cli.fan_speed_update_period)
            );
            
            device->setup_cleanup();
            
            std::cout << "Starting PI temperature control (target: "
                      << cli.target_temperature.value() << "°C)" << std::endl;

            while (true) {
                unsigned int temperature = device->get_temperature();
                unsigned int fan_speed = controller.calculate_fan_speed(temperature);
                device->set_fan_speed(fan_speed);
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
