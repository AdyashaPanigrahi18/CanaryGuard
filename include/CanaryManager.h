#pragma once
#include "CanaryFile.h"
#include <string>
#include <vector>

class CanaryManager {
public:
    explicit CanaryManager(const std::string& directory);
    std::vector<CanaryFile> createAndBaseline();

private:
    std::string directory_;
    void ensureDirectory();
    void createFile(const std::string& path, const std::string& content);
};
