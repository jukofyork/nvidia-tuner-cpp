#include "utils.h"
#include <stdexcept>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

namespace utils {

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
