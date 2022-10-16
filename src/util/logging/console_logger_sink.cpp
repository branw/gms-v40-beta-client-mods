#include "console_logger_sink.hpp"

#include <Windows.h>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>

ConsoleLoggerSink::ConsoleLoggerSink(LogLevel minimum_level) : minimum_level_(minimum_level) {
    if (!(console_already_opened_ = AttachConsole(GetCurrentProcessId()))) {
        AllocConsole();
    }

    if ((console_handle_ = GetStdHandle(STD_OUTPUT_HANDLE)) == nullptr) {
        throw std::runtime_error("Failed to get console handle");
    }

    SetConsoleOutputCP(CP_UTF8);

    console_out_.open("CONOUT$", std::ios_base::out | std::ios_base::app);
}

ConsoleLoggerSink::~ConsoleLoggerSink() {
    if (!console_already_opened_) {
        FreeConsole();
    }
}

void ConsoleLoggerSink::log(LogLevel level, std::string_view message) {
    if (level < minimum_level_) {
        return;
    }

    std::string_view level_str;
    int color;

    switch (level) {
    case LogLevel::Trace:
        level_str = "TRC ";
        color = FOREGROUND_INTENSITY;// Gray
        break;

    case LogLevel::Debug:
        level_str = "DBG ";
        color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;// Cyan
        break;

    default:
    case LogLevel::Info:
        level_str = "INFO";
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;// White
        break;

    case LogLevel::Warning:
        level_str = "WARN";
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;// Yellow
        break;

    case LogLevel::Error:
        level_str = "ERR ";
        color = FOREGROUND_RED | FOREGROUND_INTENSITY;// Red
        break;
    }

    SetConsoleTextAttribute(console_handle_, color);

    auto now = std::chrono::system_clock::now().time_since_epoch();
    console_out_ << fmt::format("{} [{:%H:%M:%S}] {}", level_str, now, message)
                 << std::endl;
}

void ConsoleLoggerSink::set_title(std::string const &message) {
    SetConsoleTitle(message.c_str());
}

std::ofstream &ConsoleLoggerSink::stream() {
    return console_out_;
}

void ConsoleLoggerSink::set_minimum_level(LogLevel minimum_level) {
    minimum_level_ = minimum_level;
}