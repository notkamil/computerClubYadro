#include "Club/Club.h"
#include "Club/Utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

namespace club {
    Club::Club(const std::string &filename) : conf(filename) {
        std::string line;
        bool is_ok = true;
        while (is_ok && std::getline(conf, line)) {
            ++line_num;
            std::vector<std::string> tokens = split(line, ' ');
            if (line_num == 1) {
                is_ok = tokens.size() == 1 && parse_positive_int(tokens[0], total_tables);
            } else if (line_num == 2) {
                is_ok = tokens.size() == 2 && parse_times(tokens[0], start_time, tokens[1], finish_time);
            } else if (line_num == 3) {
                is_ok = tokens.size() == 1 && parse_positive_int(tokens[0], hour_cost);
            } else {
                is_ok = parse_event(tokens);
            }
        }
        if (!is_ok) {
            std::cout << line << std::endl;
            return;
        }
        empty_tables = total_tables;
        sitting_client.resize(total_tables);
        sitting_start_time.resize(total_tables, -1);
        cumulative_time.resize(total_tables);
        revenue.resize(total_tables);

        handle_events();
        print_result();
    }

    Club::~Club() {
        if (conf.is_open()) {
            conf.close();
        }
    }

    std::vector<std::string> Club::split(const std::string &s, const char delim) {
        std::vector<std::string> elems;
        std::string current;
        for (int i = 0; i < s.length(); i++) {
            if (s[i] == delim) {
                elems.push_back(current);
                current.clear();
            } else {
                current.push_back(s[i]);
            }
        }
        elems.push_back(current);
        return elems;
    }

    bool Club::parse_time(const std::string &stime, int &result) {
        if (stime.size() != 5 || stime[2] != ':') {
            return false;
        }
        int hour;
        int minute;
        try {
            hour = std::stoi(stime.substr(0, 2));
            minute = std::stoi(stime.substr(3, 2));
        } catch (...) {
            return false;
        }
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
            return false;
        }
        result = hour * 60 + minute;
        return true;
    }

    bool Club::parse_times(const std::string &stime_first, int &result_first,
                           const std::string &stime_second, int &result_second) {
        return parse_time(stime_first, result_first) && parse_time(stime_second, result_second);
    }

    bool Club::is_name_symbol(char c) {
        return 'a' <= c && c <= 'z' || '0' <= c && c <= '9' || c == '_' || c == '-';
    }

    bool Club::parse_event(const std::vector<std::string> &tokens) {
        if (tokens.size() < 3) {
            return false;
        }
        int event_time;
        if (!parse_time(tokens[0], event_time) || last_event_time > event_time) {
            return false;
        }

        int event_type;
        if (!parse_positive_int(tokens[1], event_type) || event_type < 1 || event_type > 4) {
            return false;
        }
        if (event_type == 2 && tokens.size() != 4 || event_type != 2 && tokens.size() != 3) {
            return false;
        }

        std::string client_name = tokens[2];
        if (!std::all_of(tokens[2].begin(), tokens[2].end(), is_name_symbol)) {
            return false;
        }
        int table_num;
        if (event_type == 2 && (!parse_positive_int(tokens[3], table_num) || 0 > table_num || total_tables <
                                table_num)) {
            return false;
        }

        if (event_type == 1) {
            events.emplace_back(std::make_unique<EventClientCome>(event_time, client_name));
        } else if (event_type == 2) {
            events.emplace_back(std::make_unique<EventClientSit>(event_time, client_name, table_num - 1));
        } else if (event_type == 3) {
            events.emplace_back(std::make_unique<EventClientWait>(event_time, client_name));
        } else if (event_type == 4) {
            events.emplace_back(std::make_unique<EventClientLeave>(event_time, client_name));
        }
        return true;
    }

    int Club::charge_money(const int time) const {
        return (time + 59) / 60 * hour_cost;
    }

    void Club::handle_events() {
        bool kik_out_is_handled = false;
        for (auto &event: events) {
            if (!event) continue;
            if (event->get_time() > finish_time && !kik_out_is_handled) {
                handle_kick_out();
                kik_out_is_handled = true;
            }
            auto &stored_event = resulting_events.emplace_back(std::move(event));

            switch (stored_event->get_type()) {
                case 1:
                    handle_client_come(dynamic_cast<EventClientCome &>(*stored_event));
                    break;
                case 2:
                    handle_client_sit(dynamic_cast<EventClientSit &>(*stored_event));
                    break;
                case 3:
                    handle_client_wait(dynamic_cast<EventClientWait &>(*stored_event));
                    break;
                case 4:
                    handle_client_leave(dynamic_cast<EventClientLeave &>(*stored_event));
                    break;
            }
        }
        if (!kik_out_is_handled) {
            handle_kick_out();
        }
    }

    void Club::handle_client_come(const EventClientCome &event) {
        const std::string client = event.client_name;
        if (sitting_table.contains(client)) {
            resulting_events.emplace_back(std::make_unique<EventError>(event.time, "YouShallNotPass"));
            return;
        }
        if (event.time < start_time || event.time > finish_time) {
            resulting_events.emplace_back(std::make_unique<EventError>(event.time, "NotOpenYet"));
            return;
        }
        sitting_table[client] = -1;
    }

    void Club::handle_client_sit(EventClientSit &event) {
        const int new_table = event.table;
        const std::string client = event.client_name;
        if (!sitting_client[new_table].empty()) {
            resulting_events.emplace_back(std::make_unique<EventError>(event.time, "PlaceIsBusy"));
            return;
        }
        if (!sitting_table.contains(client)) {
            resulting_events.emplace_back(std::make_unique<EventError>(event.time, "ClientUnknown"));
            return;
        }
        const int old_table = sitting_table[client];
        if (old_table != -1) {
            const int old_start_time = sitting_start_time[old_table];
            const int diff_time = event.time - old_start_time;
            sitting_client[old_table] = "";
            sitting_start_time[old_table] = -1;
            sitting_table[client] = -1;
            cumulative_time[old_table] += diff_time;
            revenue[old_table] += charge_money(diff_time);
            empty_tables++;
        }

        sitting_client[new_table] = client;
        sitting_start_time[new_table] = event.time;
        sitting_table[client] = new_table;
        empty_tables--;
    }

    void Club::handle_client_wait(EventClientWait &event) {
        if (empty_tables > 0) {
            resulting_events.emplace_back(std::make_unique<EventError>(event.time, "ICanWaitNoLonger!"));
            return;
        }
        if (queue_size >= total_tables) {
            resulting_events.emplace_back(std::make_unique<EventClientKicked>(event.time, event.client_name));
            return;
        }
        queue.push(event.client_name);
        queue_size++;
    }

    void Club::handle_client_leave(EventClientLeave &event) {
        const std::string client = event.client_name;
        if (!sitting_table.contains(client)) {
            resulting_events.emplace_back(std::make_unique<EventError>(event.time, "ClientUnknown"));
            return;
        }
        const int cur_table = sitting_table[client];
        if (cur_table != -1) {
            const int old_start_time = sitting_start_time[cur_table];
            const int diff_time = event.time - old_start_time;
            sitting_client[cur_table] = "";
            sitting_start_time[cur_table] = -1;
            cumulative_time[cur_table] += diff_time;
            revenue[cur_table] += charge_money(diff_time);
        }
        sitting_table.erase(client);
        if (queue_size > 0) {
            std::string next_client = queue.front();
            queue.pop();
            --queue_size;
            sitting_client[cur_table] = next_client;
            sitting_start_time[cur_table] = event.time;
            sitting_table[next_client] = cur_table;
            resulting_events.emplace_back(std::make_unique<EventClientSat>(event.time, next_client, cur_table));
        } else {
            empty_tables++;
        }
    }

    void Club::handle_kick_out() {
        auto remaining_clients = std::vector<std::pair<std::string, int> >(
            sitting_table.begin(), sitting_table.end()
        );
        std::sort(remaining_clients.begin(), remaining_clients.end());
        for (const auto &[client, table] : remaining_clients) {
            const int old_start_time = sitting_start_time[table];
            const int diff_time = finish_time - old_start_time;
            sitting_client[table] = "";
            sitting_start_time[table] = -1;
            sitting_table[client] = -1;
            cumulative_time[table] += diff_time;
            revenue[table] += charge_money(diff_time);
            resulting_events.emplace_back(std::make_unique<EventClientKicked>(finish_time, client));
        }
    }

    void Club::print_result() const {
        std::cout << format_time(start_time) << std::endl;
        for (const auto &event: resulting_events) {
            std::cout << event->to_conf_line() << std::endl;
        }
        std::cout << format_time(finish_time) << std::endl;
        for (int i = 0; i < total_tables; i++) {
            std::cout << i + 1 << " " << revenue[i] << " " << format_time(cumulative_time[i]) << std::endl;
        }
    }

    bool Club::parse_positive_int(const std::string &sint, int &result) {
        int value;
        try {
            value = std::stoi(sint);
        } catch (...) {
            return false;
        }
        if (value < 0) {
            return false;
        }
        result = value;
        return true;
    }
} // club
