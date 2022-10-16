#include "file_logger_sink.hpp"

#include <Windows.h>
#include <fmt/chrono.h>
#include <fmt/core.h>

template<typename TP>
std::time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() +
                                                        system_clock::now());
    return system_clock::to_time_t(sctp);
}

FileLoggerSink::FileLoggerSink(const std::filesystem::path &path) {
    time_t raw_time;
    time(&raw_time);
    struct tm local_time;
    localtime_s(&local_time, &raw_time);

    auto const tagged_name = fmt::format(
            "{}-{}{:0>2}{:0>2}-{:0>2}{:0>2}{:0>2}-{}{}", path.stem().string(),
            local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday,
            local_time.tm_hour, local_time.tm_min, local_time.tm_sec,
            GetCurrentProcessId(), path.extension().string());

    auto tagged_path = path;
    tagged_path.replace_filename(tagged_name);

    file_out_.open(tagged_path, std::ios_base::out | std::ios_base::trunc);
}

void FileLoggerSink::log(LogLevel level, std::string_view message) {
    std::string_view level_str;

    switch (level) {
    case LogLevel::Trace:
        level_str = "TRC ";
        break;

    case LogLevel::Debug:
        level_str = "DBG ";
        break;

    default:
    case LogLevel::Info:
        level_str = "INFO";
        break;

    case LogLevel::Warning:
        level_str = "WARN";
        break;

    case LogLevel::Error:
        level_str = "ERR ";
        break;
    }

    auto now = std::chrono::system_clock::now().time_since_epoch();
    file_out_ << fmt::format("{} [{:%H:%M:%S}] {}", level_str, now, message)
              << std::endl;
}