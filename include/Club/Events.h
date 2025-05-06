#ifndef CLUB_EVENTS_H
#define CLUB_EVENTS_H
#include <string>

namespace club {
    class Event {
    protected:
        int time;

        explicit Event(int time);

        [[nodiscard]] std::string format_time() const;

        [[nodiscard]] std::string get_base_conf_line() const;
    public:
        virtual ~Event() = default;

        [[nodiscard]] virtual std::string to_conf_line() const = 0;

        [[nodiscard]] virtual int get_type() const = 0;

        [[nodiscard]] int get_time() const { return time; }
    };

    class EventClientCome : public Event {
        friend class Club;
        std::string client_name;

    public:
        EventClientCome(int time, std::string name);

        [[nodiscard]] std::string to_conf_line() const override;

        [[nodiscard]] int get_type() const override { return 1; }
    };

    class EventClientSit : public Event {
        friend class Club;
        std::string client_name;
        int table;

    public:
        EventClientSit(int time, std::string name, int table);

        [[nodiscard]] std::string to_conf_line() const override;

        [[nodiscard]] int get_type() const override { return 2; }
    };

    class EventClientWait : public Event {
        friend class Club;
        std::string client_name;

    public:
        EventClientWait(int time, std::string name);

        [[nodiscard]] std::string to_conf_line() const override;

        [[nodiscard]] int get_type() const override { return 3; }
    };

    class EventClientLeave : public Event {
        friend class Club;
        std::string client_name;

    public:
        EventClientLeave(int time, std::string name);

        [[nodiscard]] std::string to_conf_line() const override;

        [[nodiscard]] int get_type() const override { return 4; }
    };

    class EventClientKicked : public Event {
        friend class Club;
        std::string client_name;

    public:
        EventClientKicked(int time, std::string name);

        [[nodiscard]] std::string to_conf_line() const override;

        [[nodiscard]] int get_type() const override { return 11; }
    };

    class EventClientSat : public Event {
        friend class Club;
        std::string client_name;
        int table;

    public:
        EventClientSat(int time, std::string name, int table);

        [[nodiscard]] std::string to_conf_line() const override;

        [[nodiscard]] int get_type() const override { return 12; }
    };

    class EventError : public Event {
        friend class Club;
        std::string error;

    public:
        EventError(int time, std::string err);

        [[nodiscard]] std::string to_conf_line() const override;

        [[nodiscard]] int get_type() const override { return 13; }
    };
} // club

#endif //CLUB_EVENTS_H
