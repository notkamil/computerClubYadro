#include "Club/Utils.h"

namespace club {
    std::string format_time(const int time) {
        const int hours = time / 60;
        const int minutes = time % 60;
        std::string result;
        if (hours < 10) {
            result += "0";
        }
        result += std::to_string(hours);
        result += ":";
        if (minutes < 10) {
            result += "0";
        }
        result += std::to_string(minutes);
        return result;
    }
} // club
