#pragma once
#include "CanaryFile.h"
#include "Logger.h"
#include <atomic>
#include <vector>

class FileMonitor {
public:
    FileMonitor(std::vector<CanaryFile> files, Logger& logger);

    void run(int intervalSeconds);
    void checkOnce();
    void stop();

private:
    std::vector<CanaryFile> files_;
    Logger& logger_;
    std::atomic<bool> running_{true};

    int alertCount_{0};
    int modifiedCount_{0};
    int deletedCount_{0};
    int restoredCount_{0};

    void checkFile(CanaryFile& file);
    bool restoreFile(CanaryFile& file);
    void showStatus();
    void simulateRansomware();
};
