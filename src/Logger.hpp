#pragma once

#include <format>
#include <iostream>
#include <string>
#include <mutex>
#include <unordered_map>
#include <source_location>
#include <filesystem>

#define LOG_DEBUG(format, ...) Logs::Logger::getInstance().log(Logs::LogLevel::Debug, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Logs::Logger::getInstance().log(Logs::LogLevel::Info, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) Logs::Logger::getInstance().log(Logs::LogLevel::Warning, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Logs::Logger::getInstance().log(Logs::LogLevel::Error, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) Logs::Logger::getInstance().log(Logs::LogLevel::Fatal, std::source_location::current(), format, ##__VA_ARGS__)

namespace Logs {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

inline const char* toColor(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "\033[0;35m";   // Purple
        case LogLevel::Info: return "\033[0;36m";    // Cyan
        case LogLevel::Warning: return "\033[0;93m"; // Yellow
        case LogLevel::Error: return "\033[1;91m";   // Red
        case LogLevel::Fatal: return "\033[1;91m";   // Bright red
    }
    return "\033[0;37m";
}

inline const char* toString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Fatal: return "FATAL";
    }
    return "UNKNOWN";
}

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setMinimumLogLevel(LogLevel level) {
        minimumLogLevel = level;
    }

    template<typename... Args>
    void log(LogLevel level, const std::source_location& location, std::string_view formatStr, Args&&... args) {
        if (level < minimumLogLevel) return;

        std::string message = std::vformat(formatStr, std::make_format_args(args...));
        std::string timestamp = getTimestamp();
        auto file = std::filesystem::path(location.file_name()).filename().string();

        std::lock_guard<std::mutex> lock(logMutex);
        std::cout << timestamp
            << " [" << toColor(level) << toString(level) << "\033[0m]"
            << " [" << file << ":" << location.line() << "] "
            << message << std::endl;
    }

private:
    Logger() = default;
    std::mutex logMutex;
    LogLevel minimumLogLevel = LogLevel::Debug;

    std::string getTimestamp() const {
        std::time_t now = std::time(nullptr);
        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%F %T", std::localtime(&now));
        return buffer;
    }
};

} // namespace Logs