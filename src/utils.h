#pragma once

#include <string>

namespace utils {
    bool command_exists(const std::string& command);
    bool escalate_privileges();
}
