#ifndef MAPLE_FILE_LOGGER_SINK_HPP
#define MAPLE_FILE_LOGGER_SINK_HPP

#include "logger.hpp"

#include <filesystem>

// A logger sink that writes to an arbitrary file
class FileLoggerSink final : public LoggerSink {
    std::ofstream file_out_;

public:
    explicit FileLoggerSink(std::filesystem::path const &path);

    void log(LogLevel level, std::string_view message) override;
};

#endif//MAPLE_FILE_LOGGER_SINK_HPP
