#include "Club/Utils.h"

#include <format>

namespace club {
    std::string format_time(const int time) {
        return std::format("{:02}:{:02}", time / 60, time % 60);
    }
} // club
