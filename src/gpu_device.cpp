#include "gpu_device.h"
#include "constants.h"
#include <stdexcept>
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <string>
#include <dlfcn.h>

std::shared_ptr<NvmlDevice> NvmlDevice::cleanup_device = nullptr;

NvmlDevice::NvmlDevice(nvmlDevice_t device_handle) 
    : handle(device_handle), fan_speed_state(std::make_shared<FanSpeedState>()) {}

void NvmlDevice::set_core_clock_offset(int offset) {
    // Try to use the function, but handle if it doesn't exist
    nvmlReturn_t result = NVML_ERROR_FUNCTION_NOT_FOUND;
    
    // Check if function exists using dlsym
    typedef nvmlReturn_t (*nvmlDeviceSetGpcClkVfOffset_t)(nvmlDevice_t, int);
    static nvmlDeviceSetGpcClkVfOffset_t func = nullptr;
    static bool checked = false;
    
    if (!checked) {
        func = (nvmlDeviceSetGpcClkVfOffset_t)dlsym(RTLD_DEFAULT, "nvmlDeviceSetGpcClkVfOffset");
        checked = true;
    }
    
    if (func) {
        result = func(handle, offset);
        check_nvml_error(result, "set core clock offset");
    } else {
        throw std::runtime_error("nvmlDeviceSetGpcClkVfOffset function not available in your NVML version");
    }
}

void NvmlDevice::set_memory_clock_offset(int offset) {
    typedef nvmlReturn_t (*nvmlDeviceSetMemClkVfOffset_t)(nvmlDevice_t, int);
    static nvmlDeviceSetMemClkVfOffset_t func = nullptr;
    static bool checked = false;
    
    if (!checked) {
        func = (nvmlDeviceSetMemClkVfOffset_t)dlsym(RTLD_DEFAULT, "nvmlDeviceSetMemClkVfOffset");
        checked = true;
    }
    
    if (func) {
        nvmlReturn_t result = func(handle, offset);
        check_nvml_error(result, "set memory clock offset");
    } else {
        throw std::runtime_error("nvmlDeviceSetMemClkVfOffset function not available in your NVML version");
    }
}

void NvmlDevice::set_power_limit(unsigned int limit) {
    check_nvml_error(nvmlDeviceSetPowerManagementLimit(handle, limit * 1000), 
                    "set power limit");
}

unsigned int NvmlDevice::get_temperature() {
    unsigned int temp;
    check_nvml_error(nvmlDeviceGetTemperature(handle, NVML_TEMPERATURE_GPU, &temp), 
                    "get temperature");
    return temp;
}

unsigned int NvmlDevice::get_num_fans() {
    unsigned int num_fans;
    
    typedef nvmlReturn_t (*nvmlDeviceGetNumFans_t)(nvmlDevice_t, unsigned int*);
    static nvmlDeviceGetNumFans_t func = nullptr;
    static bool checked = false;
    
    if (!checked) {
        func = (nvmlDeviceGetNumFans_t)dlsym(RTLD_DEFAULT, "nvmlDeviceGetNumFans");
        checked = true;
    }
    
    if (func) {
        check_nvml_error(func(handle, &num_fans), "get number of fans");
        return num_fans;
    } else {
        // Fallback: assume 1 fan
        return 1;
    }
}

void NvmlDevice::set_fan_speed(unsigned int speed) {
    if (fan_speed_state->default_set.load()) {
        return;
    }
    
    typedef nvmlReturn_t (*nvmlDeviceSetFanSpeed_v2_t)(nvmlDevice_t, unsigned int, unsigned int);
    static nvmlDeviceSetFanSpeed_v2_t func = nullptr;
    static bool checked = false;
    
    if (!checked) {
        func = (nvmlDeviceSetFanSpeed_v2_t)dlsym(RTLD_DEFAULT, "nvmlDeviceSetFanSpeed_v2");
        checked = true;
    }
    
    if (func) {
        unsigned int num_fans = get_num_fans();
        for (unsigned int fan = 0; fan < num_fans; ++fan) {
            check_nvml_error(func(handle, fan, speed), "set fan speed");
        }
    } else {
        throw std::runtime_error("nvmlDeviceSetFanSpeed_v2 function not available in your NVML version");
    }
}

void NvmlDevice::set_default_fan_speed() {
    fan_speed_state->default_set.store(true);
    
    typedef nvmlReturn_t (*nvmlDeviceSetDefaultFanSpeed_v2_t)(nvmlDevice_t, unsigned int);
    static nvmlDeviceSetDefaultFanSpeed_v2_t func = nullptr;
    static bool checked = false;
    
    if (!checked) {
        func = (nvmlDeviceSetDefaultFanSpeed_v2_t)dlsym(RTLD_DEFAULT, "nvmlDeviceSetDefaultFanSpeed_v2");
        checked = true;
    }
    
    if (func) {
        unsigned int num_fans = get_num_fans();
        for (unsigned int fan = 0; fan < num_fans; ++fan) {
            nvmlReturn_t result = func(handle, fan);
            if (result != NVML_SUCCESS) {
                std::cerr << "!!! Setting the default fan speed failed on exit !!!" << std::endl;
                return;
            }
        }
        std::cout << "Successfully set default fan speed on exit!" << std::endl;
    } else {
        std::cout << "Default fan speed function not available, fan control will remain manual" << std::endl;
    }
}

void NvmlDevice::cleanup_handler(int signal) {
    std::cout << "Signal received: " << signal << std::endl;
    if (cleanup_device) {
        cleanup_device->set_default_fan_speed();
    }
    exit(0);
}

void NvmlDevice::panic_handler() {
    std::cerr << "Panic occurred!" << std::endl;
    if (cleanup_device) {
        cleanup_device->set_default_fan_speed();
    }
}

void NvmlDevice::setup_cleanup() {
    cleanup_device = shared_from_this();
    
    // Set up signal handlers for various termination signals
    std::signal(SIGINT, cleanup_handler);
    std::signal(SIGTERM, cleanup_handler);
    std::signal(SIGHUP, cleanup_handler);
    std::signal(SIGALRM, cleanup_handler);
    std::signal(SIGIO, cleanup_handler);
    std::signal(SIGPIPE, cleanup_handler);
    std::signal(SIGPROF, cleanup_handler);
    std::signal(SIGUSR1, cleanup_handler);
    std::signal(SIGUSR2, cleanup_handler);
    std::signal(SIGVTALRM, cleanup_handler);

    // Ignore stop signals
    std::signal(SIGTSTP, SIG_IGN);
    std::signal(SIGTTIN, SIG_IGN);
    std::signal(SIGTTOU, SIG_IGN);
    
    // Set terminate handler for exceptions
    std::set_terminate(panic_handler);
}

void check_nvml_error(nvmlReturn_t result, const std::string& operation) {
    if (result != NVML_SUCCESS) {
        throw std::runtime_error("Failed to " + operation + ": " + 
                               nvmlErrorString(result) + " (Error code: " + 
                               std::to_string(result) + ")");
    }
}

void check_driver_version() {
    char driver_version[NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE];
    check_nvml_error(nvmlSystemGetDriverVersion(driver_version, sizeof(driver_version)), 
                    "get driver version");
    
    std::string version_str(driver_version);
    size_t dot_pos = version_str.find('.');
    if (dot_pos == std::string::npos) {
        throw std::runtime_error("Failed to parse driver version: " + version_str);
    }
    
    int major = std::stoi(version_str.substr(0, dot_pos));
    if (major < MAJOR_MIN_VERSION) {
        throw std::runtime_error("Your driver version v" + std::to_string(major) + 
                               " is not supported. Driver version v" + 
                               std::to_string(MAJOR_MIN_VERSION) + " or newer is required.");
    }
}
