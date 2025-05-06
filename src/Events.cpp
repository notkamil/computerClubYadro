#include "Club/Events.h"
#include "Club/Utils.h"
#include <iomanip>
#include <sstream>
#include <utility>

namespace club {
    std::string Event::format_time() const {
        return club::format_time(time);
    }

    Event::Event(const int time) : time(time) {
    }

    std::string Event::get_base_conf_line() const {
        return format_time() + " " + std::to_string(get_type()) + " ";
    }

    EventClientCome::EventClientCome(const int time, std::string name)
        : Event(time), client_name(std::move(name)) {
    }

    std::string EventClientCome::to_conf_line() const {
        return get_base_conf_line() + client_name;
    }

    EventClientSit::EventClientSit(const int time, std::string name, const int table)
        : Event(time), client_name(std::move(name)), table(table) {
    }

    std::string EventClientSit::to_conf_line() const {
        return get_base_conf_line() + client_name + " " + std::to_string(table + 1);
    }

    EventClientWait::EventClientWait(const int time, std::string name)
        : Event(time), client_name(std::move(name)) {
    }

    std::string EventClientWait::to_conf_line() const {
        return get_base_conf_line() + client_name;
    }

    EventClientLeave::EventClientLeave(const int time, std::string name)
        : Event(time), client_name(std::move(name)) {
    }

    std::string EventClientLeave::to_conf_line() const {
        return get_base_conf_line() + client_name;
    }

    EventClientKicked::EventClientKicked(const int time, std::string name)
        : Event(time), client_name(std::move(name)) {
    }

    std::string EventClientKicked::to_conf_line() const {
        return get_base_conf_line() + client_name;
    }

    EventClientSat::EventClientSat(const int time, std::string name, const int table)
        : Event(time), client_name(std::move(name)), table(table) {
    }

    std::string EventClientSat::to_conf_line() const {
        return get_base_conf_line() + client_name + " " + std::to_string(table + 1);
    }

    EventError::EventError(const int time, std::string err)
        : Event(time), error(std::move(err)) {
    }

    std::string EventError::to_conf_line() const {
        return get_base_conf_line() + error;
    }
} // namespace club
