#include "CanaryManager.h"
#include "FileMonitor.h"
#include "Logger.h"

#include <algorithm>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

namespace fs = std::filesystem;

std::unique_ptr<FileMonitor> monitor;

void handleSignal(int) {
    if (monitor)
        monitor->stop();
}

int main(int argc, char* argv[]) {
    std::string baseDir = "./canary_files";
    std::string logPath = "./logs/alerts.log";
    int interval = 2;

    if (argc > 1)
        baseDir = argv[1];

    if (argc > 2)
        interval = std::max(1, std::stoi(argv[2]));

    try {
        fs::create_directories(
            fs::path(logPath).parent_path());

        Logger logger(logPath);

        logger.info(
            "================================================");

        logger.info(
            "                 CANARYGUARD");

        logger.info(
            "================================================");

        logger.info(
            "Canary directory: " +
            fs::absolute(baseDir).string());

        CanaryManager manager(baseDir);

        auto files = manager.createAndBaseline();

        logger.info(
            "Created/loaded " +
            std::to_string(files.size()) +
            " canary files.");

        for (const auto& f : files) {
            logger.info(
                "BASELINE | " +
                fs::path(f.path).filename().string() +
                " | SHA-256=" +
                f.baseline_hash);
        }

        monitor =
            std::make_unique<FileMonitor>(
                std::move(files), logger);

        std::signal(SIGINT, handleSignal);
        std::signal(SIGTERM, handleSignal);

        monitor->run(interval);

        logger.info("Monitoring stopped.");
    }
    catch (const std::exception& e) {
        std::cerr
            << "FATAL ERROR: "
            << e.what()
            << '\n';

        return 1;
    }

    return 0;
}