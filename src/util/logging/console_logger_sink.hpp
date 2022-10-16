#ifndef MAPLE_CONSOLE_LOGGER_SINK_HPP
#define MAPLE_CONSOLE_LOGGER_SINK_HPP

#include "logger.hpp"

// A logger sink that manages and prints to a Windows console window
class ConsoleLoggerSink final : public LoggerSink {
    bool console_already_opened_ = true;
    void *console_handle_;
    std::ofstream console_out_;
    LogLevel minimum_level_;

public:
    ConsoleLoggerSink(LogLevel minimum_level = LogLevel::Info);

    ~ConsoleLoggerSink();

    void log(LogLevel level, std::string_view message) override;

    void set_title(std::string const &message);

    std::ofstream &stream();

    void set_minimum_level(LogLevel minimum_level);
};

#endif//MAPLE_CONSOLE_LOGGER_SINK_HPP
