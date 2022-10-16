#ifndef MAPLE_B78564905E58496F8E8A3C670A40B102
#define MAPLE_B78564905E58496F8E8A3C670A40B102

#include "concurrent_queue.hpp"

#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <memory>
#include <mutex>
#include <string_view>
#include <vector>

enum struct LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error
};

class LoggerSink {
public:
    virtual void log(LogLevel level, std::string_view message) = 0;
};

class Logger {
    struct LogLine {
        LogLevel level;
        std::string message;
    };

    mutable std::mutex sinks_mutex_{};
    std::vector<std::unique_ptr<LoggerSink>> sinks_{};
    bool running_ = true;
    std::thread thread_;
    ConcurrentQueue<LogLine> queue_{};

public:
    Logger();

    ~Logger();

    template<typename Sink, typename... Args>
    auto &add_sink(Args &&...args) {
        std::lock_guard lock(sinks_mutex_);

        auto const &ref = sinks_.emplace_back(std::make_unique<Sink>(std::forward<Args>(args)...));
        return reinterpret_cast<Sink &>(*ref);
    }

    void log(LogLevel level, std::string const &message);

    template<typename... Args>
    void log(LogLevel level, std::string_view format_str, Args const &...args) {
        log(level, fmt::vformat(format_str, fmt::make_format_args(args...)));
    }

    template<LogLevel level, typename... Args>
    void log(std::string_view format_str, Args const &...args) {
        log(level, format_str, args...);
    }

    template<typename... Args>
    void trace(std::string_view format_str, Args const &...args) {
        log<LogLevel::Trace>(format_str, args...);
    }

    template<typename... Args>
    void debug(std::string_view format_str, Args const &...args) {
        log<LogLevel::Debug>(format_str, args...);
    }

    template<typename... Args>
    void info(std::string_view format_str, Args const &...args) {
        log<LogLevel::Info>(format_str, args...);
    }

    template<typename... Args>
    void warning(std::string_view format_str, Args const &...args) {
        log<LogLevel::Warning>(format_str, args...);
    }

    template<typename... Args>
    void error(std::string_view format_str, Args const &...args) {
        log<LogLevel::Error>(format_str, args...);
    }
};

extern std::unique_ptr<Logger> logger;

#endif//MAPLE_B78564905E58496F8E8A3C670A40B102
