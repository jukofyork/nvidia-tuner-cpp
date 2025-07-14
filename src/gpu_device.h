#pragma once

#include <memory>
#include <atomic>
#include <string>

extern "C" {
#include <nvml.h>
}

struct FanSpeedState {
    std::atomic<bool> default_set{false};
};

class NvmlDevice : public std::enable_shared_from_this<NvmlDevice> {
private:
    nvmlDevice_t handle;
    std::shared_ptr<FanSpeedState> fan_speed_state;
    
public:
    NvmlDevice(nvmlDevice_t device_handle);
    ~NvmlDevice() = default;
    
    void set_core_clock_offset(int offset);
    void set_memory_clock_offset(int offset);
    void set_max_core_clock(unsigned int clock);
    void set_max_memory_clock(unsigned int clock);
    void set_power_limit(unsigned int limit);
    unsigned int get_temperature();
    void set_fan_speed(unsigned int speed);
    void set_default_fan_speed();
    void setup_cleanup();
    
private:
    unsigned int get_num_fans();
    static void cleanup_handler(int signal);
    static void panic_handler();
    
    // Static members for cleanup
    static std::shared_ptr<NvmlDevice> cleanup_device;
};

void check_nvml_error(nvmlReturn_t result, const std::string& operation);
void check_driver_version();
