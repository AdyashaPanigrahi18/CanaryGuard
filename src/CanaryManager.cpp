#include "CanaryManager.h"
#include "HashUtils.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

CanaryManager::CanaryManager(const std::string& directory)
    : directory_(directory) {}

void CanaryManager::ensureDirectory() {
    fs::create_directories(directory_);
}

void CanaryManager::createFile(const std::string& path,
                               const std::string& content) {
    if (fs::exists(path))
        return;

    std::ofstream out(path, std::ios::binary);

    if (!out)
        throw std::runtime_error("Cannot create canary file: " + path);

    out << content << '\n';
}

std::vector<CanaryFile> CanaryManager::createAndBaseline() {
    ensureDirectory();

    const std::vector<std::pair<std::string, std::string>> specs = {
        {"canary_1.txt", "CANARY-ALERT-001: Do not modify this file."},
        {"canary_2.txt", "CANARY-ALERT-002: Protected monitoring file."},
        {"canary_3.txt", "CANARY-ALERT-003: Integrity baseline file."}
    };

    std::vector<CanaryFile> result;

    for (const auto& [name, content] : specs) {
        fs::path p = fs::path(directory_) / name;

        createFile(p.string(), content);

        std::ifstream in(p, std::ios::binary);

        if (!in)
            throw std::runtime_error("Cannot read canary file: " + p.string());

        std::ostringstream buffer;
        buffer << in.rdbuf();

        auto mtime =
            fs::last_write_time(p).time_since_epoch().count();

        result.push_back({
            p.string(),
            sha256File(p.string()),
            fs::file_size(p),
            static_cast<long long>(mtime),
            buffer.str()
        });
    }

    return result;
}
