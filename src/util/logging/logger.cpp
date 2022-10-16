#include "logger.hpp"

#include <Windows.h>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fmt/chrono.h>

std::unique_ptr<Logger> logger;

Logger::Logger() {
    thread_ = std::thread{[&] {
        while (running_) {
            auto const &&[level, message] = queue_.wait_and_pop();

            std::lock_guard lock(sinks_mutex_);
            for (auto &sink: sinks_) {
                sink->log(level, message);
            }
        }
    }};
}

Logger::~Logger() {
    running_ = false;
    thread_.join();
}

void Logger::log(LogLevel level, std::string const &message) {
    queue_.emplace(level, message);
}