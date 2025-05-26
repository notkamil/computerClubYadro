#ifndef CLUB_H
#define CLUB_H
#include <fstream>
#include <memory>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "Events.h"

namespace club {
    class Club {
        std::ifstream conf;
        std::ostream& out;
        bool is_corrupted = false;
        int total_tables = 0;
        int start_time = 0;
        int finish_time = 0;
        int hour_cost = 0;
        int line_num = 0;
        int last_event_time = 0;
        std::vector<std::unique_ptr<Event> > events;
        std::vector<std::unique_ptr<Event> > resulting_events;

        std::vector<std::string> sitting_client;
        std::vector<int> sitting_start_time;
        std::unordered_map<std::string, int> sitting_table;
        std::vector<int> cumulative_time;
        std::vector<int> revenue;
        std::deque<std::string> queue;
        int empty_tables = 0;

    public:
        explicit Club(const std::string &, std::ostream& = std::cout);

        void run();

    private:
        static std::vector<std::string> split(const std::string &, char);

        static bool parse_time(const std::string &, int &);

        static bool parse_positive_int(const std::string &, int &);

        static bool parse_times(const std::string &, int &, const std::string &, int &);

        static bool is_name_symbol(char);

        bool parse_event(const std::vector<std::string> &);

        int charge_money(int time) const;

        void handle_events();

        void handle_client_come(const EventClientCome &event);

        void handle_client_sit(EventClientSit &event);

        void handle_client_wait(EventClientWait &event);

        void handle_client_leave(EventClientLeave &event);

        void handle_kick_out();

        void print_result() const;

        void parse();
    };
} // club

#endif // CLUB_H
