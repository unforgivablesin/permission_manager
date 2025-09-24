#pragma once

#include <print>
#include <string>

enum class LogLevel { Warning, Info, Error, Debug };

class Logger {
  public:
    Logger(LogLevel level) : m_level(level) {};

    void log(std::string buffer) {
        switch (m_level) {
        case LogLevel::Warning:
            std::print("[\x1b[92mWARNING\x1b[0m] {}\n", buffer);
        case LogLevel::Error:
            std::print("[\x1b[91mERROR\x1b[0m] {}\n", buffer);
        case LogLevel::Info:
            std::print("[\x1b[93mINFO\x1b[0m] {}\n", buffer);
        case LogLevel::Debug:
            if (m_level == LogLevel::Debug)
                std::print("[\x1b[94mDEBUG\x1b[0m] {}\n", buffer);
        default:
            return;
        }
    }

  private:
    LogLevel m_level;
};

static auto logger = Logger(LogLevel::Debug);
