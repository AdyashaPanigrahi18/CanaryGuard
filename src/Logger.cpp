#include "Logger.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#else
#include <ctime>
#endif

namespace {
    std::mutex logMutex;
}

Logger::Logger(const std::string& path)
    : path_(path) {}

void Logger::write(const std::string& level,
                   const std::string& message) {

    std::lock_guard<std::mutex> lock(logMutex);

    auto now =
        std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());

    std::tm tm{};

#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif

    std::ostringstream ts;

    ts << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    std::ofstream out(path_, std::ios::app);

    if (out) {
        out << ts.str() << " | "
            << level << " | "
            << message << '\n';
    }

    std::cout << ts.str() << " | "
              << level << " | "
              << message << '\n';
}

void Logger::info(const std::string& message) {
    write("INFO", message);
}

void Logger::alert(const std::string& message) {
    write("ALERT", message);
}

void Logger::warning(const std::string& message) {
    write("WARNING", message);
}

void Logger::critical(const std::string& message) {
    write("CRITICAL", message);
}
