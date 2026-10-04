#pragma once
#include <string>

class Logger {
public:
    explicit Logger(const std::string& path);

    void info(const std::string& message);
    void alert(const std::string& message);
    void warning(const std::string& message);
    void critical(const std::string& message);

private:
    std::string path_;
    void write(const std::string& level, const std::string& message);
};
